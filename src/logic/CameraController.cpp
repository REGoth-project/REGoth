//
// Created by andre on 02.06.16.
//

#include "CameraController.h"
#include "MobController.h"
#include "PlayerController.h"
#include "engine/Input.h"
#include <components/Vob.h>
#include <components/VobClasses.h>
#include <engine/World.h>
#include <entry/input.h>
#include <components/EntityActions.h>

const float CAMERA_SMOOTHING = 10.0f;

Logic::CameraController::CameraController(World::WorldInstance& world, Handle::EntityHandle entity)
    : Controller(world, entity)
    , m_Active(true)
    , m_CameraMode(ECameraMode::ThirdPerson)
{
    memset(&m_CameraSettings, 0, sizeof(m_CameraSettings));

    //m_CameraSettings.freeCameraSettings.moveSpeed = 0.01f;
    //m_CameraSettings.freeCameraSettings.turnSpeed = 0.2f;

    //m_CameraSettings.freeCameraSettings.position = Math::float3(0.0f, 2.0f, -4.0f);

    m_CameraSettings.floatingCameraSettings.position = Math::float3(0.0f, 2.0f, -4.0f);
    m_CameraSettings.floatingCameraSettings.yaw = 0.0f;
    m_CameraSettings.floatingCameraSettings.pitch = 0.0f;

    // Init viewer
    m_CameraSettings.viewerCameraSettings.lookAt = Math::float3(0, 0, 0);
    m_CameraSettings.viewerCameraSettings.yaw = 0.0f;
    m_CameraSettings.viewerCameraSettings.pitch = 0.0f;
    m_CameraSettings.viewerCameraSettings.zoom = 30.0f;

    m_CameraSettings.thirdPersonCameraSettings.currentLookAt = Math::float3(0, 0, 0);
    m_CameraSettings.thirdPersonCameraSettings.zoomExponent = 3.2f; // initial zoom pos, feel free to modify
    m_CameraSettings.thirdPersonCameraSettings.pitch = Math::degreeToRadians(20.0f); // initial camera angle, feel free to modify
    // if you want to see more of the hero's legs, decrease this value
    m_CameraSettings.thirdPersonCameraSettings.cameraElevation = Math::degreeToRadians(25.0f);
    m_CameraSettings.thirdPersonCameraSettings.deltaPhi = 0;

    // FirstPerson action
    {
        using namespace Engine;
        auto& settings = m_CameraSettings.floatingCameraSettings;
        auto& firstPerson = m_CameraSettings.firstPersonCameraSettings;

        firstPerson.actionMoveForward = registerBinding(ECameraMode::FirstPerson, ActionType::FirstPersonMoveForward, [&settings](bool, float intensity) {
            settings.position += 0.1f * intensity * settings.forward;
        });
        firstPerson.actionMoveRight = registerBinding(ECameraMode::FirstPerson, ActionType::FirstPersonMoveRight, [&settings](bool, float intensity) {
            settings.position -= 0.1f * intensity * settings.right;
        });
        firstPerson.actionLookHorizontal = registerBinding(ECameraMode::FirstPerson, ActionType::FirstPersonLookHorizontal, [&settings](bool, float intensity) {
            settings.yaw += 0.02f * intensity;
        });
        firstPerson.actionLookVertical = registerBinding(ECameraMode::FirstPerson, ActionType::FirstPersonLookVertical, [&settings](bool, float intensity) {
            settings.pitch += 0.02f * intensity;
        });
    }

    // ThirdPerson action
    {
        using namespace Engine;
        auto& settings = m_CameraSettings.thirdPersonCameraSettings;

        settings.actionWheel = registerBinding(ECameraMode::ThirdPerson, ActionType::ThirdPersonMouseWheel, [&settings](bool triggered, float intensity) {
            if (triggered)
            {
                auto& zoom = settings.zoomExponent;
                zoom = Math::clamp(zoom - intensity / 4, 0.0f, 15.0f);
            }
        });

        settings.actionLookVertical = registerBinding(ECameraMode::ThirdPerson, ActionType::ThirdPersonLookVertical, [&settings](bool, float intensity) {
            settings.pitch += 0.02f * intensity;
            auto max = Math::PI / 2 - settings.cameraElevation;
            constexpr auto epsilon = 0.05f;
            settings.pitch = Math::clamp(settings.pitch, -Math::PI / 2 * (1 - epsilon), max);
        });

        settings.actionLookHorizontal = registerBinding(ECameraMode::ThirdPerson, ActionType::ThirdPersonLookHorizontal, [&settings](bool, float intensity) {
            settings.deltaPhi += 0.02f * intensity; // TODO window width influences this???
        });
    }

    // Free Actions
    {
        using namespace Engine;
        auto& settings = m_CameraSettings.floatingCameraSettings;
        auto& free = m_CameraSettings.freeCameraSettings;

        free.actionMoveForward = registerBinding(ECameraMode::Free, ActionType::FreeMoveForward, [&settings, this](bool, float intensity) {
            settings.position += 0.07f * intensity * settings.forward * m_moveSpeedMultiplier;
        });
        free.actionMoveRight = registerBinding(ECameraMode::Free, ActionType::FreeMoveRight, [&settings, this](bool, float intensity) {
            settings.position -= 0.1f * intensity * settings.right * m_moveSpeedMultiplier;
        });
        free.actionMoveUp = registerBinding(ECameraMode::Free, ActionType::FreeMoveUp, [&settings, this](bool, float intensity) {
            settings.position += 0.1f * intensity * settings.up * m_moveSpeedMultiplier;
        });
        free.actionLookHorizontal = registerBinding(ECameraMode::Free, ActionType::FreeLookHorizontal, [&settings, this](bool, float intensity) {
            settings.yaw += 0.02f * intensity;

            // Rotate direction-vector, if wanted // TODO: Refractor

            // Get player position
            VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_World, m_World.getScriptEngine().getPlayerEntity());

            if (player.isValid() && !player.playerController->getUsedMob().isValid())
            {
                m_CameraSettings.thirdPersonCameraSettings.currentOffsetDirection =
                    Math::Matrix::CreateRotationY(0.02f * intensity) *
                    m_CameraSettings.thirdPersonCameraSettings.currentOffsetDirection;
            }
        });
        free.actionLookVertical = registerBinding(ECameraMode::Free, ActionType::FreeLookVertical, [&settings](bool, float intensity) {
            settings.pitch += 0.02f * intensity;
        });
    }

    // Viewer actions
    {
        using namespace Engine;
        auto& settings = m_CameraSettings.viewerCameraSettings;

        settings.actionViewHorizontal = registerBinding(ECameraMode::Viewer, ActionType::ViewerHorizontal, [&settings](bool, float intensity) {
            if (settings.isRotateModifier)
            {
                // Neither pan nor zoom activated, or both -> Rotate
                if (settings.isPanModifier == settings.isZoomModifier)
                    settings.yaw += 0.05f * intensity;
                else if (settings.isPanModifier)
                    settings.lookAt -= 0.01 * settings.zoom * intensity * settings.right;
            }
        });
        settings.actionViewVertical = registerBinding(ECameraMode::Viewer, ActionType::ViewerVertical, [&settings](bool, float intensity) {
            if (settings.isRotateModifier)
            {
                // Neither pan nor zoom activated, or both -> Rotate
                if (settings.isPanModifier == settings.isZoomModifier)
                    settings.pitch += 0.05f * intensity;
                else if (settings.isPanModifier)
                    settings.lookAt += 0.01f * settings.zoom * intensity * settings.up;
                else if (settings.isZoomModifier)
                {
                    settings.zoom += 10.0f * intensity;
                    if (settings.zoom < 1.0f)
                        settings.zoom = 1.0f;
                }
            }
        });
        settings.actionPan = registerBinding(ECameraMode::Viewer, ActionType::ViewerPan, [&settings](bool triggered, float) {
            settings.isPanModifier = triggered;
        });
        settings.actionZoom = registerBinding(ECameraMode::Viewer, ActionType::ViewerZoom, [&settings](bool triggered, float) {
            settings.isZoomModifier = triggered;
        });
        settings.actionRotate = registerBinding(ECameraMode::Viewer, ActionType::ViewerRotate, [&settings](bool triggered, float) {
            settings.isRotateModifier = triggered;
        });
        settings.actionClick = registerBinding(ECameraMode::Viewer, ActionType::ViewerClick, [this, &settings](bool triggered, float) {
            if (triggered)
            {
                constexpr float maxRayLength = 1000.0f;
                Math::float2 mousePosition = Input::getMouseCoordinates();

                Math::float3 cameraSpaceRayEndpoint = maxRayLength * Math::float3(mousePosition.x, -mousePosition.y, 1.0f);
                Math::float3 to = m_ViewMatrix.Invert() * cameraSpaceRayEndpoint;
                Math::float3 from = settings.lookAt + settings.zoom * settings.in;
                Physics::RayTestResult hit = m_World.getPhysicsSystem().raytrace(from, to);

                if (hit.hasHit)
                {
                    settings.lookAt = hit.hitPosition;
                    Math::float3 distance = from - hit.hitPosition;
                    settings.zoom = distance.length();
                    settings.pitch = asin(distance.y / settings.zoom);
                    settings.yaw = atan2(distance.x, distance.z);
                }
            }
        });
        settings.actionWheel = registerBinding(ECameraMode::Viewer, ActionType::ViewerMouseWheel, [&settings](bool triggered, float intensity) {
            if (triggered)
            {
                settings.zoom -= 3.0 * intensity;
                if (settings.zoom < 1.0f)
                    settings.zoom = 1.0f;
            }
        });
    }

    // Disable all at first and wait until one is enabled.
    disableActions();
}

void Logic::CameraController::disableActions()
{
    Engine::Input::setMouseLock(false);
    m_CameraSettings.firstPersonCameraSettings.actionMoveForward->setEnabled(false);
    m_CameraSettings.firstPersonCameraSettings.actionMoveRight->setEnabled(false);
    m_CameraSettings.firstPersonCameraSettings.actionLookHorizontal->setEnabled(false);
    m_CameraSettings.firstPersonCameraSettings.actionLookVertical->setEnabled(false);

    m_CameraSettings.thirdPersonCameraSettings.actionWheel->setEnabled(false);
    m_CameraSettings.thirdPersonCameraSettings.actionLookVertical->setEnabled(false);
    m_CameraSettings.thirdPersonCameraSettings.actionLookHorizontal->setEnabled(false);

    m_CameraSettings.freeCameraSettings.actionMoveForward->setEnabled(false);
    m_CameraSettings.freeCameraSettings.actionMoveRight->setEnabled(false);
    m_CameraSettings.freeCameraSettings.actionMoveUp->setEnabled(false);
    m_CameraSettings.freeCameraSettings.actionLookHorizontal->setEnabled(false);
    m_CameraSettings.freeCameraSettings.actionLookVertical->setEnabled(false);

    m_CameraSettings.viewerCameraSettings.actionViewHorizontal->setEnabled(false);
    m_CameraSettings.viewerCameraSettings.actionViewVertical->setEnabled(false);
    m_CameraSettings.viewerCameraSettings.actionPan->setEnabled(false);
    m_CameraSettings.viewerCameraSettings.actionZoom->setEnabled(false);
    m_CameraSettings.viewerCameraSettings.actionRotate->setEnabled(false);
    m_CameraSettings.viewerCameraSettings.actionClick->setEnabled(false);
    m_CameraSettings.viewerCameraSettings.actionWheel->setEnabled(false);
}

void Logic::CameraController::onUpdateExplicit(float deltaTime)
{
    if (!m_Active)
        return;  // TODO: Should do automatic movement anyways!

    switch (m_CameraMode)
    {
        case ECameraMode::ThirdPerson:
        {
            // Get player position
            VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_World, m_World.getScriptEngine().getPlayerEntity());

            if (player.isValid())
            {
                const float verticalFactor = std::sin(m_CameraSettings.thirdPersonCameraSettings.cameraElevation);
                const float horizontalFactor = std::cos(m_CameraSettings.thirdPersonCameraSettings.cameraElevation);
                // TODO use movestate direction instead? (swimming not tested)
                Math::Matrix pTrans = player.playerController->getEntityTransformFacing();
                Math::float3 pdir;

                // If player is currently using a mob check if camera should be locked
                // If so, use last known position and finish rotating to it
                VobTypes::MobVobInformation mob = VobTypes::asMobVob(m_World, player.playerController->getUsedMob());
                if (!mob.isValid())
                {
                    pdir = pTrans.Forward();
                }
                else if (!mob.mobController->isCameraLocked())
                {
                    pdir = pTrans.Forward();
                    m_savedPdir = pdir;
                }
                else
                    pdir = m_savedPdir;

                const float interpolationFraction = std::min(CAMERA_SMOOTHING * deltaTime, 1.0f);
                m_CameraSettings.thirdPersonCameraSettings.currentOffsetDirection = Math::float3::lerp(m_CameraSettings.thirdPersonCameraSettings.currentOffsetDirection,
                                                                                                       pdir,
                                                                                                       interpolationFraction);

                pdir = m_CameraSettings.thirdPersonCameraSettings.currentOffsetDirection;


                Components::AnimationComponent& anim = Components::Actions::initComponent<Components::AnimationComponent>(player.world->getComponentAllocator(), player.entity);
                const auto& playerSize = anim.getAnimHandler().getMeshLib().getBBoxMax();
                const auto& width = playerSize.x;
                const auto& height = playerSize.y;
                const auto& length = playerSize.z;
                float playerDimension = (width + height + length) / 3;

                const auto& playerCenter = pTrans.Translation();

                const Math::float3 up = Math::float3(0.0f, 1.0f, 0.0f);

                float angle = m_CameraSettings.thirdPersonCameraSettings.pitch;
                const auto& elevation = m_CameraSettings.thirdPersonCameraSettings.cameraElevation;
                auto actualCameraAngle = Math::radiansToDegree(angle + elevation);

                auto rotationAxisDir = pTrans.Left();

                // cardinalPoint around which the camera will rotate vertically
                auto cameraRotationCenter = playerCenter;
                const auto& zoomExponent = m_CameraSettings.thirdPersonCameraSettings.zoomExponent;
                float zoom = std::exp(zoomExponent * playerDimension);

                Math::float3 newLookAt = cameraRotationCenter + verticalFactor * zoom * up;
                Math::float3 newCamPos = newLookAt - horizontalFactor * zoom * pdir;

                auto& deltaPhi = m_CameraSettings.thirdPersonCameraSettings.deltaPhi;
                for (auto p : {&newLookAt, &newCamPos})
                {
                    *p = Math::Matrix::rotatedPointAroundLine(*p, cameraRotationCenter, rotationAxisDir, angle);
                    // rotate camera around y-axis
                    // *p = Math::Matrix::rotatedPointAroundLine(*pc, pTrans.Translation(), pTrans.Up(), deltaPhi);
                }

                Math::float3 oldCamPos = getEntityTransform().Translation();
                Math::float3 intCamPos = Math::float3::lerp(oldCamPos, newCamPos, interpolationFraction);

                const Math::float3& oldLookAt = m_CameraSettings.thirdPersonCameraSettings.currentLookAt;
                m_CameraSettings.thirdPersonCameraSettings.currentLookAt = Math::float3::lerp(oldLookAt, newLookAt,
                                                                                              interpolationFraction);

                m_ViewMatrix = Math::Matrix::CreateLookAt(intCamPos,
                                                          m_CameraSettings.thirdPersonCameraSettings.currentLookAt,
                                                          up);

                setEntityTransform(m_ViewMatrix.Invert());
            }
        }
        break;

        case ECameraMode::FirstPerson:
        {
            auto& settings = m_CameraSettings.floatingCameraSettings;

            // Get forward/right vector
            std::tie(settings.forward, settings.right) = getDirectionVectors(settings.yaw, settings.pitch);
            settings.up = settings.right.cross(settings.forward);
            settings.forward *= deltaTime;

            // Fix position
            Math::float3 to = settings.position + Math::float3(0.0f, -100.0f, 0.0f);
            Physics::RayTestResult hit = m_World.getPhysicsSystem().raytrace(settings.position, to);

            if (hit.hasHit)
            {
                settings.position = hit.hitPosition + Math::float3(0.0f, 1.8f, 0.0f);
            }

            m_ViewMatrix = Math::Matrix::CreateView(settings.position,
                                                    settings.yaw,
                                                    settings.pitch);

            setEntityTransform(m_ViewMatrix.Invert());
        }
        break;

        case ECameraMode::Free:
        {
            auto& settings = m_CameraSettings.floatingCameraSettings;

            // Get forward/right vector
            std::tie(settings.forward, settings.right) = getDirectionVectors(settings.yaw, settings.pitch);
            settings.up = settings.right.cross(settings.forward);

            settings.forward *= deltaTime * 40.0f;

            m_ViewMatrix = Math::Matrix::CreateView(settings.position,
                                                    settings.yaw,
                                                    settings.pitch);

            setEntityTransform(m_ViewMatrix.Invert());
        }
        break;

        case ECameraMode::Viewer:
        {
            auto& settings = m_CameraSettings.viewerCameraSettings;

            // getDirectionVectors only returns 2 of 3 direction vectors
            std::tie(settings.in, settings.right) = getDirectionVectors(settings.yaw, settings.pitch);
            settings.up = settings.right.cross(settings.in);

            m_ViewMatrix = Math::Matrix::CreateLookAt(
                settings.lookAt + settings.zoom * settings.in, settings.lookAt, settings.up);
            setEntityTransform(m_ViewMatrix.Invert());
        }
        break;
        case ECameraMode::Static:
        {
            //TODO add handling there?
        }
        break;
    }
}

std::pair<Math::float3, Math::float3> Logic::CameraController::getDirectionVectors(float yaw, float pitch)
{
    Math::float3 direction(
        cos(pitch) * sin(yaw),
        sin(pitch),
        cos(pitch) * cos(yaw));

    // Right vector
    Math::float3 right(
        sin(yaw - 3.14f / 2.0f),
        0,
        cos(yaw - 3.14f / 2.0f));

    return std::make_pair(direction.normalize(), right.normalize());
}

void Logic::CameraController::setTransforms(const Math::float3& position, float yaw, float pitch)
{
    m_CameraSettings.floatingCameraSettings.position = position;
    m_CameraSettings.floatingCameraSettings.yaw = yaw;
    m_CameraSettings.floatingCameraSettings.pitch = pitch;
}

void Logic::CameraController::setCameraMode(Logic::CameraController::ECameraMode mode)
{
    m_CameraMode = mode;
    disableActions();
    switch (mode)
    {
        case ECameraMode::FirstPerson:
            Engine::Input::setMouseLock(true);
            m_CameraSettings.firstPersonCameraSettings.actionMoveForward->setEnabled(true);
            m_CameraSettings.firstPersonCameraSettings.actionMoveRight->setEnabled(true);
            m_CameraSettings.firstPersonCameraSettings.actionLookHorizontal->setEnabled(true);
            m_CameraSettings.firstPersonCameraSettings.actionLookVertical->setEnabled(true);
            break;
        case ECameraMode::Free:
            Engine::Input::setMouseLock(true);
            m_CameraSettings.freeCameraSettings.actionMoveForward->setEnabled(true);
            m_CameraSettings.freeCameraSettings.actionMoveRight->setEnabled(true);
            m_CameraSettings.freeCameraSettings.actionMoveUp->setEnabled(true);
            m_CameraSettings.freeCameraSettings.actionLookHorizontal->setEnabled(true);
            m_CameraSettings.freeCameraSettings.actionLookVertical->setEnabled(true);
            break;
        case ECameraMode::Viewer:
            Engine::Input::setMouseLock(false);
            m_CameraSettings.viewerCameraSettings.actionViewHorizontal->setEnabled(true);
            m_CameraSettings.viewerCameraSettings.actionViewVertical->setEnabled(true);
            m_CameraSettings.viewerCameraSettings.actionPan->setEnabled(true);
            m_CameraSettings.viewerCameraSettings.actionZoom->setEnabled(true);
            m_CameraSettings.viewerCameraSettings.actionRotate->setEnabled(true);
            m_CameraSettings.viewerCameraSettings.actionClick->setEnabled(true);
            m_CameraSettings.viewerCameraSettings.actionWheel->setEnabled(true);
            break;
        case ECameraMode::ThirdPerson:
            Engine::Input::setMouseLock(true);
            m_CameraSettings.thirdPersonCameraSettings.actionWheel->setEnabled(true);
            m_CameraSettings.thirdPersonCameraSettings.actionLookVertical->setEnabled(true);
            m_CameraSettings.thirdPersonCameraSettings.actionLookHorizontal->setEnabled(true);
            break;
    }
    #ifndef NDEBUG
    Engine::Input::setMouseLock(false);
    #endif
}

void Logic::CameraController::clearBindings()
{
    m_ActionBindings.clear();
}

Logic::CameraController::~CameraController()
{
    clearBindings();
}
