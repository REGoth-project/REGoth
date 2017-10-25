#pragma once
#include <unordered_map>
#include "zenload/zCModelAni.h"
#include "zenload/zCModelMeshLib.h"
#include <handle/HandleDef.h>
#include <math/mathlib.h>

namespace World
{
    class WorldInstance;
}

namespace Animations
{
    struct Animation;
}

namespace ZenLoad
{
    struct zCModelScriptEventSfx;
    struct zCModelScriptEventTag;
    struct zCModelScriptEventPfx;
}

namespace Components
{
    // TOOD: in gothic Model Scripts control everything related, rename
    class AnimHandler
    {
    public:
        AnimHandler();

        /**
         * Access to the world this resides in
         */
        void setWorld(World::WorldInstance& world) { m_pWorld = &world; }
        /**
         * @brief Sets the mesh-lib this operates on. Does a copy inside, so the given object can be deleted.
         */
        void setMeshLib(const ZenLoad::zCModelMeshLib& meshLib);

        /**
         * @brief Loads a mesh-lib from the given VDF-Index
         */
        bool loadMeshLibFromVDF(const std::string& file, VDFS::FileIndex& idx);

        /**
         * @brief Adds an animation to the library. Does a copy inside, so the given object can be deleted.
         *		  TODO: Should not copy the animation-samples...
         */
        //void addAnimation(const ZenLoad::zCModelAni& ani);
        bool addAnimation(const std::string& name);

        /**
         * @brief Sets the currently playing animation. Restarts it, if this is currently running. Doesn't loop.
         */
        void playAnimation(const std::string& animName);

        /**
         * @brief Sets the currently playing animation. Restarts it, if this is currently running. Doesn't loop.
         */
        void playAnimation(Handle::AnimationHandle anim);

        /**
         * @brief Sets the currently playing animation without restarting it, if it is currently running. Loops.
         */
        void setAnimation(const std::string& animName);

        /**
         * @brief Sets the overlay for this animation manager
         */
        void setOverlay(const std::string& mds);

        /**
         * @brief Updates the currently playing animations
         */
        void updateAnimations(double deltaTime);

        /**
         * @brief Stops the current animation and sets the bindpose
         * @param force If this is set to false, this method will do nothing of there isn't currently an animation running
         */
        void setBindPose(bool force = false);
        void stopAnimation() { setBindPose(); }
        /**
         * @brief Draws the skeleton of the animation
         */
        void debugDrawSkeleton(const Math::Matrix& transform);

        /**
         * @brief Updates the given skeletalmesh-info with data calculated from the current animation frames.
         * 		  Use this to get the packed state of the animationController.
         */
        void updateSkeletalMeshInfo(Math::Matrix* target, size_t numMatrices);

        /**
         * @return Velocity of the root node in m/s
         */
        Math::float3 getRootNodeVelocityTotal();
        Math::float3 getRootNodeVelocity() { return m_AnimRootVelocity; };
        const Math::Matrix& getRootNodeRotationVelocity() { return m_AnimRootRotationVelocity; }
        /**
         * @return Animation-velocity averaged over a few frames
         */
        Math::float3 getRootNodeVelocityAvg();

        /**
         * Returns the position of the node at the given frame
         * Note: -1 = lastFrame
         */
        Math::float3 getRootNodePositionAt(size_t frame);

        /**
         * @return Root-node position on the current frame
         */
        Math::float3 getRootNodePosition();

        /**
         * @return the currently active animation. nullptr if none is active. Do not save this pointer, as it can change!
         */
        Animations::Animation* getActiveAnimationPtr();
        Handle::AnimationHandle getAcitveAnimation() { return m_ActiveAnimation; }
        /**
         * @return Value in range 0..1 telling how far we are with playing the active animation
         */
        float getActiveAnimationProgress();

        /**
         * @return Number of nodes in this skeleton
         */
        size_t getNumNodes()
        {
            return m_MeshLib.getNodes().size();
        }

        /**
         * @return Last state of the finalized object space node-transforms
         */
        const std::vector<Math::Matrix>& getObjectSpaceTransforms()
        {
            return m_ObjectSpaceNodeTransforms;
        }

        /**
         * @return The mesh lib containing the skeleton
         */
        const ZenLoad::zCModelMeshLib& getMeshLib()
        {
            return m_MeshLib;
        }

        /**
         * @return Name of the currently loaded mesh-lib
         */
        const std::string& getMeshLibName()
        {
            return m_MeshLibName;
        }

        /**
         * @return Currently active overlay
         */
        const std::string& getOverlay()
        {
            return m_ActiveOverlay;
        }

        /**
         * Checks whether the given animation is available
         * @param name Animation to check
         * @return whether the given animation is available
         */
        bool hasAnimation(const std::string& name)
        {
            return m_AnimationsByName.find(name) != m_AnimationsByName.end();
        }

        /**
         * @return Value useful to check if there was an actual change. This value is modified every time
         * 		  the animation was updated
         */
        size_t getAnimationStateHash()
        {
            return m_AnimationStateHash;
        }

        /**
         * @return Whether the animation-root velocity was updated this frame
         */
        bool hasUpdatedAnimRootVelocity() { return m_AnimRootNodeVelocityUpdatedHash == getAnimationStateHash(); }
        /**
         * @return Animation-object from the handle
         */
        Animations::Animation& getAnimation(Handle::AnimationHandle h);

        /**
         * Sets the speed multiplier for all animations
         */
        void setSpeedMultiplier(float mult) { m_SpeedMultiplier = mult; }
        /**
         * Event-Callbacks
         */
        void setCallbackEventSFX(std::function<void(const ZenLoad::zCModelScriptEventSfx& sfx)> cb)
        {
            m_CallbackTriggerSFX = cb;
        }

        void setCallbackEventSFXGround(std::function<void(const ZenLoad::zCModelScriptEventSfx& sfx)> cb)
        {
            m_CallbackTriggerSFXGround = cb;
        }

        void setCallbackEventTag(std::function<void(const ZenLoad::zCModelScriptEventTag& tag)> cb)
        {
            m_CallbackTriggerEventTag = cb;
        }
        void setCallbackEventPfx(std::function<void(const ZenLoad::zCModelScriptEventPfx& pfx)> cb)
        {
            m_CallbackTriggerPFX = cb;
        }

    private:
        /**
         * Checks if any events happened between the last played frames
         * @param frameLast Animation-Frame played in the last update-cycle
         * @param frameNow Animation-Frame played in this update-cycle
         */
        void triggerEvents(int frameLast, int frameNow);

        /**
         * Plays the given SFX-Event
         * @param sfx SFX t play
         */
        std::function<void(const ZenLoad::zCModelScriptEventSfx& sfx)> m_CallbackTriggerSFX;
        std::function<void(const ZenLoad::zCModelScriptEventSfx& sfx)> m_CallbackTriggerSFXGround;
        std::function<void(const ZenLoad::zCModelScriptEventTag& tag)> m_CallbackTriggerEventTag;

        /**
         * Plays the given PFX-Event
         * @param PFX to play
         */
        std::function<void(const ZenLoad::zCModelScriptEventPfx& pfx)> m_CallbackTriggerPFX;

        /**
         * @brief Animations by their name
         */
        std::vector<Handle::AnimationHandle> m_Animations;
        std::unordered_map<std::string, Handle::AnimationHandle> m_AnimationsByName;

        /**
         * @brief Meshlib this operates on
         */
        ZenLoad::zCModelMeshLib m_MeshLib;
        std::string m_MeshLibName;

        /** 
         * @brief Active animation
         */
        Handle::AnimationHandle m_ActiveAnimation;
        float m_AnimationFrame;
        size_t m_LastProcessedFrame;
        bool m_LoopActiveAnimation;

        /**
         * @brief Active overlay
         */
        std::string m_ActiveOverlay;

        /** 
         * @brief Node transforms in local space
         */
        std::vector<Math::Matrix> m_NodeTransforms;

        /**
         * @brief Node transforms in object-space
         */
        std::vector<Math::Matrix> m_ObjectSpaceNodeTransforms;

        /**
         * @brief Root-Node-Veclocity in m/s
         */
        Math::float3 m_AnimRootVelocity;
        Math::Matrix m_AnimRootRotationVelocity;
        size_t m_AnimRootNodeVelocityUpdatedHash;  // AnimHash when this was last updated
        Math::float3 m_AnimRootPosition;
        Math::Matrix m_AnimRootRotation;

        /**
         * Smoothes the model-velocity over a few frames
         */
        enum
        {
            NUM_VELOCITY_AVERAGE_STEPS = 8
        };
        Math::float3 m_AnimVelocityRingBuff[NUM_VELOCITY_AVERAGE_STEPS];
        unsigned m_AnimVelocityRingCurrent = 0;

        /**
         * @brief Value useful to check if there was an actual change. This value is modified every time
         * 		  the animation was updated
         */
        size_t m_AnimationStateHash;

        /**
         * @brief World this resides in
         */
        World::WorldInstance* m_pWorld;

        /**
         * @brief Speed multiplier for all animations
         */
        float m_SpeedMultiplier;
    };
}
