#pragma once
#include "Controller.h"

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
            ThirdPerson
        };

        struct CameraSettings
        {
            /**
             * Settings for camera mode "free"
             */
            struct
            {
                float turnSpeed;
                float moveSpeed;
                float yaw, pitch;
                Math::float3 position;
            } freeCameraSettings;

            /**
             * Settings for camera mode static
             */
            struct
            {
                Math::float3 position;
                Math::float3 lookAt;
            } staticCameraSettings;

            /**
             * Settings for camera mode "firstperson"
             */
            struct
            {
                float turnSpeed;
                float moveSpeed;
                float yaw, pitch;
            } firstPersonCameraSettings;

            /**
             * Settings for camera mode "thirdperson"
             */
            struct
            {
                float turnSpeed;
                float turnSmoothness;
                float yaw, height, distance;
            } thirdPersonCameraSettings;
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

        Math::float2 m_LastMousePosition;
    };
}