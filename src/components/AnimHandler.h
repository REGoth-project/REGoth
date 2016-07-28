#pragma once
#include "zenload/zCModelAni.h"
#include "zenload/zCModelMeshLib.h"
#include <unordered_map>
#include <math/mathlib.h>
#include <handle/HandleDef.h>

namespace World
{
	class WorldInstance;
}

namespace Animations
{
	struct Animation;
}

namespace Components
{
	class AnimHandler
	{
	public:

		AnimHandler();

		/**
		 * Access to the world this resides in
		 */
		void setWorld(World::WorldInstance& world){ m_pWorld = &world; }

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
		bool addAnimation(const std::string& file);

		/**
		 * @brief Sets the currently playing animation. Restarts it, if this is currently running
		 */
		void playAnimation(const std::string& animName);

		/**
		 * @brief Sets the currently playing animation without restarting it, if it is currently running
		 */
		void setAnimation(const std::string& animName);

		/**
		 * @brief Updates the currently playing animations
		 */
		void updateAnimations(double deltaTime);

		/**
		 * @brief Stops the current animation and sets the bindpose
		 * @param force If this is set to false, this method will do nothing of there isn't currently an animation running
		 */
		void setBindPose(bool force=false);
		void stopAnimation(){setBindPose();}

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
		const Math::float3& getRootNodeVelocity(){ return m_AnimRootVelocity; }

		/**
		 * @return the currently active animation. nullptr if none is active. Do not save this pointer, as it can change!
		 */
		ZenLoad::zCModelAni* getActiveAnimationPtr();

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
		 * @return Animation-object from the handle
		 */
		Animations::Animation& getAnimation(Handle::AnimationHandle h);
	private:

		/**
		 * @brief Animations by their name
		 */
		std::vector<Handle::AnimationHandle> m_Animations;
		std::unordered_map<std::string, Handle::AnimationHandle> m_AnimationsByName;

		/**
		 * @brief Meshlib this operates on
		 */
		ZenLoad::zCModelMeshLib m_MeshLib;

		/** 
		 * @brief Active animation
		 */
		Handle::AnimationHandle m_ActiveAnimation;
		float m_AnimationFrame;
		size_t m_LastProcessedFrame;

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

		/**
		 * @brief Value useful to check if there was an actual change. This value is modified every time
		 * 		  the animation was updated
		 */
		size_t m_AnimationStateHash;

		/**
		 * @brief World this resides in
		 */
		World::WorldInstance* m_pWorld;
	};
}