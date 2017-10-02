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

const float CAMERA_SMOOTHING = 4.0f;

Logic::CameraController::CameraController(World::WorldInstance& world, Handle::EntityHandle entity)
    : Controller(world, entity)
    , m_Active(true)
    , m_CameraMode(ECameraMode::FirstPerson)
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

    m_KeyframeDuration = 1.0f;
}

void Logic::CameraController::setupKeybinds()
{
    // FirstPerson action
    {
        using namespace Engine;
        auto& settings = m_CameraSettings.floatingCameraSettings;
        auto& firstPerson = m_CameraSettings.firstPersonCameraSettings;

        firstPerson.actionMoveForward = Input::RegisterAction(ActionType::FirstPersonMoveForward, [&settings](bool, float intensity) {
            settings.position += 0.1f * intensity * settings.forward;
        });
        firstPerson.actionMoveRight = Input::RegisterAction(ActionType::FirstPersonMoveRight, [&settings](bool, float intensity) {
            settings.position -= 0.1f * intensity * settings.right;
        });
        firstPerson.actionLookHorizontal = Input::RegisterAction(ActionType::FirstPersonLookHorizontal, [&settings](bool, float intensity) {
            settings.yaw += 0.02f * intensity;
        });
        firstPerson.actionLookVertical = Input::RegisterAction(ActionType::FirstPersonLookVertical, [&settings](bool, float intensity) {
            settings.pitch += 0.02f * intensity;
        });
    }

    // Free Actions
    {
        using namespace Engine;
        auto& settings = m_CameraSettings.floatingCameraSettings;
        auto& free = m_CameraSettings.freeCameraSettings;

        free.actionMoveForward = Input::RegisterAction(ActionType::FreeMoveForward, [&settings, this](bool, float intensity) {
            settings.position += 0.07f * intensity * settings.forward * m_moveSpeedMultiplier;
        });
        free.actionMoveRight = Input::RegisterAction(ActionType::FreeMoveRight, [&settings, this](bool, float intensity) {
            settings.position -= 0.1f * intensity * settings.right * m_moveSpeedMultiplier;
        });
        free.actionMoveUp = Input::RegisterAction(ActionType::FreeMoveUp, [&settings, this](bool, float intensity) {
            settings.position += 0.1f * intensity * settings.up * m_moveSpeedMultiplier;
        });
        free.actionLookHorizontal = Input::RegisterAction(ActionType::FreeLookHorizontal, [&settings, this](bool, float intensity) {
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
        free.actionLookVertical = Input::RegisterAction(ActionType::FreeLookVertical, [&settings](bool, float intensity) {
            settings.pitch += 0.02f * intensity;
        });
    }

    // Viewer actions
    {
        using namespace Engine;
        auto& settings = m_CameraSettings.viewerCameraSettings;

        settings.actionViewHorizontal = Input::RegisterAction(ActionType::ViewerHorizontal, [&settings](bool, float intensity) {
            if (settings.isRotateModifier)
            {
                // Neither pan nor zoom activated, or both -> Rotate
                if (settings.isPanModifier == settings.isZoomModifier)
                    settings.yaw += 0.05f * intensity;
                else if (settings.isPanModifier)
                    settings.lookAt -= 0.01 * settings.zoom * intensity * settings.right;
            }
        });
        settings.actionViewVertical = Input::RegisterAction(ActionType::ViewerVertical, [&settings](bool, float intensity) {
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
        settings.actionPan = Input::RegisterAction(ActionType::ViewerPan, [&settings](bool triggered, float) {
            settings.isPanModifier = triggered;
        });
        settings.actionZoom = Input::RegisterAction(ActionType::ViewerZoom, [&settings](bool triggered, float) {
            settings.isZoomModifier = triggered;
        });
        settings.actionRotate = Input::RegisterAction(ActionType::ViewerRotate, [&settings](bool triggered, float) {
            settings.isRotateModifier = triggered;
        });
        settings.actionClick = Input::RegisterAction(ActionType::ViewerClick, [this, &settings](bool triggered, float) {
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
        settings.actionWheel = Input::RegisterAction(ActionType::ViewerMouseWheel, [&settings](bool triggered, float intensity) {
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

    Engine::Input::RegisterAction(Engine::ActionType::CameraFirstPerson, [this](bool triggered, float) {
        if (triggered)
        {
            disableActions();
            m_CameraMode = ECameraMode::FirstPerson;
            Engine::Input::setMouseLock(true);
            m_CameraSettings.firstPersonCameraSettings.actionMoveForward->setEnabled(true);
            m_CameraSettings.firstPersonCameraSettings.actionMoveRight->setEnabled(true);
            m_CameraSettings.firstPersonCameraSettings.actionLookHorizontal->setEnabled(true);
            m_CameraSettings.firstPersonCameraSettings.actionLookVertical->setEnabled(true);
        }
    });
    Engine::Input::RegisterAction(Engine::ActionType::CameraFree, [this](bool triggered, float) {
        if (triggered)
        {
            disableActions();
            m_CameraMode = ECameraMode::Free;
            Engine::Input::setMouseLock(true);
            m_CameraSettings.freeCameraSettings.actionMoveForward->setEnabled(true);
            m_CameraSettings.freeCameraSettings.actionMoveRight->setEnabled(true);
            m_CameraSettings.freeCameraSettings.actionMoveUp->setEnabled(true);
            m_CameraSettings.freeCameraSettings.actionLookHorizontal->setEnabled(true);
            m_CameraSettings.freeCameraSettings.actionLookVertical->setEnabled(true);
        }
    });
    Engine::Input::RegisterAction(Engine::ActionType::CameraViewer, [this](bool triggered, float) {
        if (triggered)
        {
            disableActions();
            //m_CameraMode = ECameraMode::Viewer;
            m_CameraMode = ECameraMode::KeyedAnimation;
            m_KeyframeActive = 0.0f;
            Engine::Input::setMouseLock(false);
            m_CameraSettings.viewerCameraSettings.actionViewHorizontal->setEnabled(true);
            m_CameraSettings.viewerCameraSettings.actionViewVertical->setEnabled(true);
            m_CameraSettings.viewerCameraSettings.actionPan->setEnabled(true);
            m_CameraSettings.viewerCameraSettings.actionZoom->setEnabled(true);
            m_CameraSettings.viewerCameraSettings.actionRotate->setEnabled(true);
            m_CameraSettings.viewerCameraSettings.actionClick->setEnabled(true);
            m_CameraSettings.viewerCameraSettings.actionWheel->setEnabled(true);
        }
    });
    Engine::Input::RegisterAction(Engine::ActionType::CameraThirdPerson, [this](bool triggered, float) {
        if (triggered && m_World.getScriptEngine().getPlayerEntity().isValid())
        {
            disableActions();
            m_CameraMode = ECameraMode::ThirdPerson;
            Engine::Input::setMouseLock(false);
        }
    });

    Engine::Input::RegisterAction(Engine::ActionType::DebugMoveSpeed, [this](bool triggered, float intensity) {
        m_moveSpeedMultiplier = triggered ? 5.0f : 1.0f;
    });
}

void Logic::CameraController::disableActions()
{
    m_CameraSettings.firstPersonCameraSettings.actionMoveForward->setEnabled(false);
    m_CameraSettings.firstPersonCameraSettings.actionMoveRight->setEnabled(false);
    m_CameraSettings.firstPersonCameraSettings.actionLookHorizontal->setEnabled(false);
    m_CameraSettings.firstPersonCameraSettings.actionLookVertical->setEnabled(false);

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
                Math::Matrix ptrans = Vob::getTransform(player);

                Math::float3 pdir;

                // If player is currently using a mob check if camera should be locked
                // If so, use last known position and finish rotating to it
                VobTypes::MobVobInformation mob = VobTypes::asMobVob(m_World, player.playerController->getUsedMob());
                if (!mob.isValid())
                {
                    pdir = -1.0f * ptrans.Forward();
                }
                else if (!mob.mobController->isCameraLocked())
                {
                    pdir = -1.0f * ptrans.Forward();
                    m_savedPdir = pdir;
                }
                else
                    pdir = m_savedPdir;

                m_CameraSettings.thirdPersonCameraSettings.currentOffsetDirection = Math::float3::lerp(m_CameraSettings.thirdPersonCameraSettings.currentOffsetDirection,
                                                                                                       pdir,
                                                                                                       std::min(1.0f, CAMERA_SMOOTHING * deltaTime));

                pdir = m_CameraSettings.thirdPersonCameraSettings.currentOffsetDirection;

                const float bestRange = 2.5f;
                const float bestElevation = 25.0f;  // Degrees
                //const Math::float3 targetOffset = Math::float3(0.0f, 2.0f, 0.0f);
                //const Math::float3 cameraOffset = Math::float3(0.0f, 1.0f, 0.0f);
                const Math::float3 targetOffset = Math::float3(0.0f, 1.0f, 0.0f);
                const Math::float3 cameraOffset = Math::float3(0.0f, 1.2f, 0.0f);

                Math::float3 ppos = ptrans.Translation() - pdir * bestRange + cameraOffset;  // Camera position

                Math::float3 camPos = getEntityTransform().Translation();

                ppos = Math::float3::lerp(camPos, ppos, std::min(1.0f, CAMERA_SMOOTHING * deltaTime));

                m_CameraSettings.thirdPersonCameraSettings.currentLookAt = Math::float3::lerp(m_CameraSettings.thirdPersonCameraSettings.currentLookAt,
                                                                                              ptrans.Translation() + targetOffset,
                                                                                              std::min(1.0f, CAMERA_SMOOTHING * deltaTime));

                m_ViewMatrix = Math::Matrix::CreateLookAt(ppos,
                                                          m_CameraSettings.thirdPersonCameraSettings.currentLookAt,
                                                          Math::float3(0, 1, 0));

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

        case ECameraMode::KeyedAnimation:
        {
            if(!m_Keyframes.empty() && m_KeyframeActive != -1.0f)
            {
                std::pair<Math::float3, Math::float3> poslookat = updateKeyframedPlay(deltaTime);
                m_ViewMatrix = Math::Matrix::CreateLookAt(
                        poslookat.first, poslookat.first + poslookat.second, Math::float3(0, 1, 0));
                setEntityTransform(m_ViewMatrix.Invert());
            }
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

void Logic::CameraController::storeKeyframe(unsigned idx)
{
    Keyframe f;
    f.position = m_ViewMatrix.Invert().Translation();
    f.lookat = -1.0f * m_ViewMatrix.Invert().Forward();

    if(idx >= m_Keyframes.size())
        m_Keyframes.resize(idx + 1, f);

    m_Keyframes[idx] = f;
}

void Logic::CameraController::clearKeyframes()
{
    m_Keyframes.clear();
}

void Logic::CameraController::playKeyframes(float speed)
{
    m_KeyframeDuration = speed;
    m_KeyframeActive = 0.0f;
    setCameraMode(ECameraMode::KeyedAnimation);
}

std::pair<Math::float3, Math::float3> Logic::CameraController::updateKeyframedPlay(float dt)
{
    if(m_KeyframeActive == -1.0f)
        return std::make_pair(m_Keyframes.back().position, m_Keyframes.back().lookat);

    m_KeyframeActive += dt;

    float frame = (m_KeyframeActive / (m_KeyframeDuration * 2.0f)) * m_Keyframes.size();

    float frac = fmod(frame, 1.0f);
    int current = (int)frame;
    int next = current + 1;

    if(current + 1 >= (int)m_Keyframes.size())
    {
        m_KeyframeActive = -1.0f;
        return std::make_pair(m_Keyframes.back().position, m_Keyframes.back().lookat);
    }

    Math::float3 pos    = Math::float3::lerp(m_Keyframes[current].position, m_Keyframes[next].position, frac);
    Math::float3 lookat = Math::float3::lerp(m_Keyframes[current].lookat, m_Keyframes[next].lookat, frac);

    return std::make_pair(pos, lookat);
}
