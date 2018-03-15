#pragma once
#include "Controller.h"
#include <engine/Input.h>

namespace Logic
{
    class CameraController : public Controller
    {
    public:
        ~CameraController();

        enum class ECameraMode
        {
            ThirdPerson,
            FirstPerson,
            Dialog,
            Free,
            Viewer, // name is open to change
            Static,
            KeyedAnimation,
        };

        struct CameraSettings
        {
            /**
             * Settings for camera mode "free"
             */
            struct
            {
                Engine::Action* actionMoveForward;
                Engine::Action* actionMoveRight;
                Engine::Action* actionMoveUp;
                Engine::Action* actionLookHorizontal;
                Engine::Action* actionLookVertical;
            } freeCameraSettings;

            /**
             * Settings for camera mode "firstperson"
             */
            struct
            {
                Engine::Action* actionLookHorizontal;
                Engine::Action* actionLookVertical;

                float yaw, pitch;
            } firstPersonCameraSettings;

            /**
             * Settings for camera mode "thirdperson"
             */
            struct
            {
                Engine::Action* actionWheel;
                Engine::Action* actionLookVertical;
                Engine::Action* actionLookHorizontal;

                Math::float3 currentLookAt;
                Math::float3 currentOffsetDirection;
                float zoomExponent;
                // vertical angle of the camera: 0° = behind player horizontal. 90° = looking from above (-y direction)
                float pitch;
                // angle between camera's look-at-point and rotation center. 0° = camera looks at rotation center
                float cameraElevation;
                // rotation around vertical axis (y) to be done on next camera update
                float deltaPhi;
            } thirdPersonCameraSettings;

            struct
            {
                float yaw, pitch;
                Math::float3 position;
                Math::float3 up, right, forward;
            } floatingCameraSettings;

            /**
             * Settings for camera mode "viewer"
             */
            struct
            {
                Engine::Action* actionViewHorizontal;
                Engine::Action* actionViewVertical;
                Engine::Action* actionPan;
                Engine::Action* actionZoom;
                Engine::Action* actionRotate;
                Engine::Action* actionClick;
                Engine::Action* actionWheel;

                bool isPanModifier;
                bool isZoomModifier;
                bool isRotateModifier;

                Math::float3 lookAt;
                float mouseWheel;
                Math::float3 up, right, in;
                float yaw, pitch, zoom;
            } viewerCameraSettings;
        };

        /**
         * @param world World of the underlaying entity
         * @param entity Entity owning this controller
         */
        CameraController(World::WorldInstance& world, Handle::EntityHandle entity);

        /**
         * @return The type of this class. If you are adding a new base controller, be sure to add it to ControllerTypes.h
         */
        EControllerType getControllerType() override { return EControllerType::CameraController; }
        /**
         * Called on game-tick
         */
        void onUpdateExplicit(float deltaTime);

        /**
         * @brief Sets how the camera should behave
         */
        void setCameraMode(ECameraMode mode);

        /**
         * Returns current camera mode
         * @return ECameraMode camera mode
         */
        ECameraMode getCameraMode() {
            return m_CameraMode;
        }

        /**
         * Sets camera mode to mode before last setCameraMode() call
         */
        void restoreCameraMode()
        {
            setCameraMode(m_savedCameraMode);
        }


        /**
         * @brief Sets whether this controller should read input
         */
        void setActive(bool active);

        /**
         * Sets the entity to follow in the first/third person camera modes
         */
        void followEntity(Handle::EntityHandle h)
        {
            m_FollowedEntity = h;
        }

        /**
         * @return access to the settings of this camera
         */
        CameraSettings& getCameraSettings()
        {
            return m_CameraSettings;
        }

        /**
         * Registers all inputs inside the engine
         */
        void setupKeybinds();

        /**
         * Sets the transform of this camera
         */
        void setTransforms(const Math::float3& position, float yaw = 0.0f, float pitch = 0.0f);

        void setDebugMoveSpeed(float moveSpeedMultiplier) { m_moveSpeedMultiplier = moveSpeedMultiplier; }

        /**
         * Stores a keyframe with the current camera postion/rotation
         * @param idx Index of the keyframe to store
         */
        void storeKeyframe(unsigned idx);

        /**
         * Clears all stored keyframes
         */
        void clearKeyframes();

        /**
         * Plays all stored keyframes
         */
        void playKeyframes(float duration = 1.0f);


    protected:

        /**
         * Moves the camera according to the stored keyframes
         * @return (Position, lookat)
         */
        std::pair<Math::float3, Math::float3> updateKeyframedPlay(float dt);

        /**
         * registers a binding
         */
        template <class Functor>
        Engine::Action* registerBinding(ECameraMode cameraMode, Engine::ActionType actionType, Functor functor)
        {
            auto managedBinding = Engine::Input::RegisterAction(actionType, functor);
            m_ActionBindings[cameraMode].push_back(std::move(managedBinding));
            return &m_ActionBindings[cameraMode].back().getAction();
        }

        /**
         * clears all bindings for camera steering
         */
        void clearBindings();

        /**
         * Transforms the given yaw/pitch into the corresponding direction vectors
         * @return pair of (forward, right)
         */
        std::pair<Math::float3, Math::float3> getDirectionVectors(float yaw, float pitch);

        /**
         * Enables/Disables key bindings for firstPerson and thirdPerson camera mode
         * @param enable
         */
        void enableActions(bool enable);

        /**
         * Enables key bindings for given mode, disables for other modes
         * @param mode determines what keybindings are enabled
         */
        void switchModeActions(ECameraMode mode);


        /**
         * Whether this controller should read player input
         */
        bool m_Active;

        /**
         * How the camera should behave regarding the followed entity
         */
        ECameraMode m_CameraMode;

        /**
         * Remember camera mode (i.e. for dialog) to restore it later
         */
        ECameraMode m_savedCameraMode;

        /**
         * Entity this is attached to
         */
        Handle::EntityHandle m_FollowedEntity;

        /**
         * Settings for the different camera modes
         */
        CameraSettings m_CameraSettings;

        /**
         * stored bindings
         */
        std::map<ECameraMode, std::vector<Engine::ManagedActionBinding>> m_ActionBindings;

        /**
         * Current view-matrix
         */
        Math::Matrix m_ViewMatrix;

        //Math::float2 m_LastMousePosition;

        /**
         * Debug
         */
        float m_moveSpeedMultiplier;

        struct Keyframe
        {
            Math::float3 position;
            Math::float3 lookat;
        };

        /**
         * List of keyframes currently set
         */
        std::vector<Keyframe> m_Keyframes;

        /**
         * Currently active keyframe (fraction). -1 = no keyframe active
         */
        float m_KeyframeActive;
        float m_KeyframeDuration;


        Daedalus::GameState::NpcHandle m_DialogTarget;

        /**
             * Direction to use during locked camera while using mobs
             */
        Math::float3 m_savedPdir;
    };
}
