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
#include <engine/GameSession.h>
#include <physics/PhysicsSystem.h>
#include <logic/ScriptEngine.h>

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
    m_CameraSettings.thirdPersonCameraSettings.pitch = Math::degreeToRadians(0.0f); // initial camera angle, feel free to modify
                                                                                    // if you want to see more of the hero's legs, decrease this value
    m_CameraSettings.thirdPersonCameraSettings.cameraElevation = Math::degreeToRadians(25.0f);
    m_CameraSettings.thirdPersonCameraSettings.deltaPhi = 0;

    m_CameraSettings.firstPersonCameraSettings.pitch = 0;
    m_CameraSettings.firstPersonCameraSettings.yaw = 0;

    m_CameraSettings.dialogueCameraSettings.dialogueShotCounter = 0;
    m_CameraSettings.dialogueCameraSettings.dialogueShotLimit = 2;
    m_CameraSettings.dialogueCameraSettings.dontShowHeroChance = 4;

    m_KeyframeDuration = 1.0f;
    setupKeybinds();
}

void Logic::CameraController::setupKeybinds()
{
    // FirstPerson action
    {
        using namespace Engine;
        auto& settings = m_CameraSettings.firstPersonCameraSettings;

        settings.actionLookHorizontal = registerBinding(ECameraMode::FirstPerson, ActionType::FirstPersonLookHorizontal, [&settings](bool, float intensity) {
            settings.yaw += 0.02f * intensity;
            settings.yaw = Math::clamp(settings.yaw, -Math::PI / 2, Math::PI / 2);
        });
        settings.actionLookVertical = registerBinding(ECameraMode::FirstPerson, ActionType::FirstPersonLookVertical, [&settings](bool, float intensity) {
            settings.pitch += 0.02f * intensity;
            settings.pitch = Math::clamp(settings.pitch, -Math::PI / 2, Math::PI / 2);
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
            settings.position += 0.1f * intensity * settings.forward * m_moveSpeedMultiplier;
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
    enableActions(false);
    Engine::Input::setMouseLock(false);
}

void Logic::CameraController::setActive(bool active)
{
    m_Active = active;
    enableActions(active);
}

void Logic::CameraController::enableActions(bool enable)
{
    for (auto& pair : m_ActionBindings)
    {
        if (pair.first == m_CameraMode)
        {
            for (auto &managedBinding : pair.second) {
                managedBinding.getAction().setEnabled(enable);
            }
        }
    }
}

void Logic::CameraController::switchModeActions(ECameraMode mode)
{
    for (auto& pair : m_ActionBindings)
    {
        bool enable = pair.first == mode;
        for (auto& managedBinding : pair.second)
        {
            managedBinding.getAction().setEnabled(enable);
        }
    }
}

void Logic::CameraController::nextDialogueShot() {
    VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_World,
                                                            m_World.getScriptEngine().getPlayerEntity());

    EDialogueShotType nextShot = m_DialogueShotType;
    bool playerTalking = m_dialogueTargetName == player.playerController->getScriptInstance().name[0];

    // Rule: Use close-up and neutral only after at least two fulls or shoulders
    // Rule: Only full or over-the-shoulder shot for PC_Hero
    if (playerTalking || m_CameraSettings.dialogueCameraSettings.dialogueShotCounter <=
                         m_CameraSettings.dialogueCameraSettings.dialogueShotLimit) {
        // Rule: don't always cut to PC_Hero when they speak. Leave chance for camera to stay on target NPC
        m_dontShowHero = rand() % m_CameraSettings.dialogueCameraSettings.dontShowHeroChance != 0;
        if (m_CameraSettings.dialogueCameraSettings.dialogueShotCounter == 0 ||
            m_CameraSettings.dialogueCameraSettings.dialogueShotCounter >
            m_CameraSettings.dialogueCameraSettings.dialogueShotLimit) {

            // Only choose from first two (Full and OverTheShoulder)
            nextShot = (EDialogueShotType) (rand() % 2);
        }
    } else {
        // Rule: A close-up is the only possible option after a neutral shot
        if (m_DialogueShotType == EDialogueShotType::Neutral && rand() % 4 == 0)
            nextShot = EDialogueShotType::CloseUp;
            // Rule: No shot should come after a close-up
        else if (m_DialogueShotType != EDialogueShotType::CloseUp)
            nextShot = (EDialogueShotType) (rand() % 4);
    }

    m_DialogueShotType = nextShot;
    m_CameraSettings.dialogueCameraSettings.dialogueShotCounter++;
}

void Logic::CameraController::updateDialogueCamera() {
    VobTypes::NpcVobInformation npc_vob = VobTypes::getVobFromScriptHandle(m_World, m_dialogueTargetNPCHandle);

    VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_World,
                                                            m_World.getScriptEngine().getPlayerEntity());

    Math::Matrix pTrans = player.playerController->getEntityTransform();
    Math::Matrix npcTrans = npc_vob.playerController->getEntityTransform();
    Math::Matrix otherTrans, targetTrans;

    // Can be either 1 or -1, flip the camera based on which character is speaking
    float reverseShotModifier;

    if (m_dialogueTargetName == player.playerController->getScriptInstance().name[0] && m_dontShowHero) {
        // The PC_Hero is talking
        otherTrans = npcTrans;
        targetTrans = pTrans;
        reverseShotModifier = -1.0;
    } else {
        // The other character is talking
        otherTrans = pTrans;
        targetTrans = npcTrans;
        reverseShotModifier = 1.0;
    }

    // Pull further back based on distance between characters
    float distance = (pTrans.Translation() - npcTrans.Translation()).length();

    switch (m_DialogueShotType) {
        case EDialogueShotType::Full: {
            m_ViewMatrix = targetTrans.RotatedAroundLine(targetTrans.Translation(), targetTrans.Right(), 0);
            m_ViewMatrix *= Math::Matrix::CreateTranslation(-1.5 * reverseShotModifier, 0.5, distance + 0.3);
            Math::Matrix targetLookAt = targetTrans * Math::Matrix::CreateTranslation(0.0, 0.6, 0.0);
            m_ViewMatrix = Math::Matrix::CreateLookAt(m_ViewMatrix.Translation(), targetLookAt.Translation(),
                                                      otherTrans.Up());
            m_ViewMatrix = m_ViewMatrix.Invert();
            m_ViewMatrix = m_ViewMatrix.RotatedAroundLine(m_ViewMatrix.Translation(), m_ViewMatrix.Up(),
                                                          -0.3 * reverseShotModifier);
        }
            break;
        case EDialogueShotType::OverTheShoulder: {
            m_ViewMatrix = otherTrans.RotatedAroundLine(otherTrans.Translation(), otherTrans.Right(), 0.2);
            m_ViewMatrix *= Math::Matrix::CreateTranslation(0.5 * reverseShotModifier, 0.8, -0.6);
            m_ViewMatrix = m_ViewMatrix.RotatedAroundLine(m_ViewMatrix.Translation(), otherTrans.Up(),
                                                          -0.55 * reverseShotModifier);
        }
            break;
        case EDialogueShotType::Neutral: {
            m_ViewMatrix = npcTrans.RotatedAroundLine(npcTrans.Translation(), npcTrans.Up(), Math::PI / 2);
            m_ViewMatrix *= Math::Matrix::CreateTranslation((distance / 2) * -1, 0.5, distance * -1);
        }
            break;
        case EDialogueShotType::CloseUp: {
            m_ViewMatrix = targetTrans.RotatedAroundLine(targetTrans.Translation(), targetTrans.Up(), Math::PI);
            m_ViewMatrix *= Math::Matrix::CreateTranslation(-0.3 * reverseShotModifier, 0.8, -1.5);
            m_ViewMatrix = m_ViewMatrix.RotatedAroundLine(otherTrans.Translation(), otherTrans.Right(), 0.2);
            m_ViewMatrix = m_ViewMatrix.RotatedAroundLine(m_ViewMatrix.Translation(), otherTrans.Up(),
                                                          0.2 * reverseShotModifier);
        }
            break;
    }
}

void Logic::CameraController::updateThirdPersonCamera(float deltaTime) {
    VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_World, m_World.getScriptEngine().getPlayerEntity());

    if (player.isValid()) {
        const float verticalFactor = std::sin(m_CameraSettings.thirdPersonCameraSettings.cameraElevation);
        const float horizontalFactor = std::cos(m_CameraSettings.thirdPersonCameraSettings.cameraElevation);
        // TODO use movestate direction instead? (swimming not tested)
        Math::Matrix pTrans = player.playerController->getEntityTransformFacing();
        Math::float3 pdir;

        // If player is currently using a mob check if camera should be locked
        // If so, use last known position and finish rotating to it
        VobTypes::MobVobInformation mob = VobTypes::asMobVob(m_World, player.playerController->getUsedMob());
        if (!mob.isValid()) {
            pdir = pTrans.Forward();
        } else if (!mob.mobController->isCameraLocked()) {
            pdir = pTrans.Forward();
            m_savedPdir = pdir;
        } else
            pdir = m_savedPdir;

        const float interpolationFraction = std::min(CAMERA_SMOOTHING * deltaTime, 1.0f);
        m_CameraSettings.thirdPersonCameraSettings.currentOffsetDirection = Math::float3::lerp(
                m_CameraSettings.thirdPersonCameraSettings.currentOffsetDirection,
                pdir,
                interpolationFraction);

        pdir = m_CameraSettings.thirdPersonCameraSettings.currentOffsetDirection;


        Components::AnimationComponent &anim = Components::Actions::initComponent<Components::AnimationComponent>(
                player.world->getComponentAllocator(), player.entity);
        const auto &playerSize = anim.getAnimHandler().getMeshLib().getBBoxMax();
        const auto &width = playerSize.x;
        const auto &height = playerSize.y;
        const auto &length = playerSize.z;
        float playerDimension = (width + height + length) / 3;

        const auto &playerCenter = pTrans.Translation();

        const Math::float3 up = Math::float3(0.0f, 1.0f, 0.0f);

        float angle = m_CameraSettings.thirdPersonCameraSettings.pitch;
        const auto &elevation = m_CameraSettings.thirdPersonCameraSettings.cameraElevation;
        auto actualCameraAngle = Math::radiansToDegree(angle + elevation);

        auto rotationAxisDir = pTrans.Left();

        // cardinalPoint around which the camera will rotate vertically
        auto cameraRotationCenter = playerCenter;
        const auto &zoomExponent = m_CameraSettings.thirdPersonCameraSettings.zoomExponent;
        float zoom = std::exp(zoomExponent * playerDimension);

        Math::float3 newLookAt = cameraRotationCenter + verticalFactor * zoom * up;
        Math::float3 newCamPos = newLookAt - horizontalFactor * zoom * pdir;

        auto &deltaPhi = m_CameraSettings.thirdPersonCameraSettings.deltaPhi;
        for (auto p : {&newLookAt, &newCamPos}) {
            *p = Math::Matrix::rotatedPointAroundLine(*p, cameraRotationCenter, rotationAxisDir, angle);
            // rotate camera around y-axis
            // *p = Math::Matrix::rotatedPointAroundLine(*pc, pTrans.Translation(), pTrans.Up(), deltaPhi);
        }

        Math::float3 oldCamPos = getEntityTransform().Translation();
        Math::float3 intCamPos = Math::float3::lerp(oldCamPos, newCamPos, interpolationFraction);

        const Math::float3 &oldLookAt = m_CameraSettings.thirdPersonCameraSettings.currentLookAt;
        m_CameraSettings.thirdPersonCameraSettings.currentLookAt = Math::float3::lerp(oldLookAt, newLookAt,
                                                                                      interpolationFraction);

        m_ViewMatrix = Math::Matrix::CreateLookAt(intCamPos,
                                                  m_CameraSettings.thirdPersonCameraSettings.currentLookAt,
                                                  up);

        m_ViewMatrix = m_ViewMatrix.Invert();
    }
}

void Logic::CameraController::updateFirstPersonCamera()
{
    VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_World,
                                                            m_World.getScriptEngine().getPlayerEntity());

    if (player.isValid()) {
        auto &settings = m_CameraSettings.firstPersonCameraSettings;
        Math::Matrix pTrans = player.playerController->getEntityTransform();
        // TODO find position of player's head
        m_ViewMatrix = pTrans.RotatedAroundLine(pTrans.Translation(), pTrans.Right(), settings.pitch);
    }
}

void Logic::CameraController::updateFreeCamera(float deltaTime)
{
    auto &settings = m_CameraSettings.floatingCameraSettings;

    // Get forward/right vector
    std::tie(settings.forward, settings.right) = getDirectionVectors(settings.yaw, settings.pitch);
    settings.up = settings.right.cross(settings.forward);

    settings.forward *= deltaTime * 100.0f;
    settings.right *= deltaTime * 100.0f;

    m_ViewMatrix = Math::Matrix::CreateView(settings.position,
                                            settings.yaw,
                                            settings.pitch);

    m_ViewMatrix = m_ViewMatrix.Invert();
}

void Logic::CameraController::updateViewerCamera()
{
    auto &settings = m_CameraSettings.viewerCameraSettings;

    // getDirectionVectors only returns 2 of 3 direction vectors
    std::tie(settings.in, settings.right) = getDirectionVectors(settings.yaw, settings.pitch);
    settings.up = settings.right.cross(settings.in);

    m_ViewMatrix = Math::Matrix::CreateLookAt(
            settings.lookAt + settings.zoom * settings.in, settings.lookAt, settings.up);
    m_ViewMatrix = m_ViewMatrix.Invert();
}

void Logic::CameraController::updateKeyedAnimationCamera(float deltaTime) {
    if (m_Keyframes.empty() && m_KeyframeActive == -1.0f) {
        return;
    }

    std::pair<Math::float3, Math::float3> poslookat = updateKeyframedPlay(deltaTime);
    m_ViewMatrix = Math::Matrix::CreateLookAt(
            poslookat.first, poslookat.first + poslookat.second, Math::float3(0, 1, 0));
    m_ViewMatrix = m_ViewMatrix.Invert();
}

void Logic::CameraController::onUpdateExplicit(float deltaTime)
{
    Math::Matrix nextViewMatrix;
    switch (m_CameraMode) {
        case ECameraMode::Dialogue:
            updateDialogueCamera();
            break;
        case ECameraMode::ThirdPerson:
            updateThirdPersonCamera(deltaTime);
            break;
        case ECameraMode::FirstPerson:
            updateFirstPersonCamera();
            break;
        case ECameraMode::Free:
            updateFreeCamera(deltaTime);
            break;
        case ECameraMode::Viewer:
            updateViewerCamera();
            break;
        case ECameraMode::KeyedAnimation:
            updateKeyedAnimationCamera(deltaTime);
            break;
        case ECameraMode::Static:
            //TODO add handling there?
            break;
    }
    setEntityTransform(m_ViewMatrix);
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
    m_savedCameraMode = m_CameraMode;
    m_CameraMode = mode;
    switchModeActions(mode);
    switch (mode)
    {
        case ECameraMode::FirstPerson:
            Engine::Input::setMouseLock(true);
            // TODO disable player visual rendering
            break;
        case ECameraMode::Free:
            Engine::Input::setMouseLock(true);
            break;
        case ECameraMode::Viewer:
            Engine::Input::setMouseLock(false);
            break;
        case ECameraMode::ThirdPerson:
            Engine::Input::setMouseLock(true);
            break;
        case ECameraMode::Dialogue:
            Engine::Input::setMouseLock(true);
            break;
        case ECameraMode::KeyedAnimation:
            m_KeyframeActive = 0.0f;
            Engine::Input::setMouseLock(false);
            break;
        case ECameraMode::Static:
            Engine::Input::setMouseLock(false);
            break;
    }
    bool enablePlayerBindings = false
        || (mode == ECameraMode::FirstPerson)
        || (mode == ECameraMode::ThirdPerson)
        || (mode == ECameraMode::Static);
    m_World.getEngine()->getSession().enablePlayerBindings(enablePlayerBindings);
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
    if (speed <= 0.0f)
        return;
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
