#pragma once
#include "Controller.h"
#include <engine/Input.h>

namespace Logic
{
    class CameraController : public Controller
    {
    public:

        enum class ECameraMode
        {
            Free,
            Static,
            FirstPerson,
            ThirdPerson,
            Viewer // name is open to change
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
                Engine::Action* actionMoveForward;
                Engine::Action* actionMoveRight;
                Engine::Action* actionLookHorizontal;
                Engine::Action* actionLookVertical;
            } firstPersonCameraSettings;

//            /**
//             * Settings for camera mode "thirdperson"
//             */
//            struct
//            {
//                float turnSpeed;
//                float turnSmoothness;
//                float yaw, height, distance;
//            } thirdPersonCameraSettings;

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
         * Called on game-tick
         */
        void onUpdateExplicit(float deltaTime);

        /**
         * @brief Sets how the camera should behave
         */
        void setCameraMode(ECameraMode mode)
        {
            m_CameraMode = mode;
        }

        /**
         * @brief Sets whether this controller should read input
         */
        void setActive(bool active)
        {
            m_Active = active;
        }

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
         * Sets the transform of this camera
         */
        void setTransforms(const Math::float3& position, float yaw = 0.0f, float pitch = 0.0f);
    protected:

        /**
         * Transforms the given yaw/pitch into the corresponding direction vectors
         * @return pair of (forward, right)
         */
        std::pair<Math::float3, Math::float3> getDirectionVectors(float yaw, float pitch);

        /**
         * Whether this controller should read player input
         */
        bool m_Active;

        void disableActions();

        /**
         * How the camera should behave regarding the followed entity
         */
        ECameraMode m_CameraMode;

        /**
         * Entity this is attached to
         */
        Handle::EntityHandle m_FollowedEntity;

        /**
         * Settings for the different camera modes
         */
        CameraSettings m_CameraSettings;

        /**
         * Current view-matrix
         */
        Math::Matrix m_ViewMatrix;

        //Math::float2 m_LastMousePosition;

        /**
         * Debug
         */
        float m_moveSpeedMultiplier;
    };
}
