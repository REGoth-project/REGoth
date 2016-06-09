#pragma once
#include "zenload/zCModelAni.h"
#include "zenload/zCModelMeshLib.h"
#include <unordered_map>
#include <math/mathlib.h>

namespace Components
{
	class AnimHandler
	{
	public:

		AnimHandler();

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
		void addAnimation(const ZenLoad::zCModelAni& ani);
		bool addAnimation(const std::string& file, VDFS::FileIndex& idx, float scale = 1.0f / 100.0f);

		/**
		 * @brief Sets the currently playing animation
		 */
		void playAnimation(const std::string& animName);

		/**
		 * @brief Updates the currently playing animations
		 */
		void updateAnimations(double deltaTime);

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
		 * @return the currently active animation. nullptr if none is active
		 */
		ZenLoad::zCModelAni* getActiveAnimation();

		/**
		 * @return Number of nodes in this skeleton
		 */
		size_t getNumNodes()
		{
			return m_MeshLib.getNodes().size();
		}
	private:

		/**
		 * @brief Animations by their name
		 */
		std::vector<ZenLoad::zCModelAni> m_Animations;
		std::unordered_map<std::string, size_t> m_AnimationsByName;

		/**
		 * @brief Meshlib this operates on
		 */
		ZenLoad::zCModelMeshLib m_MeshLib;

		/** 
		 * @brief Active animation
		 */
		size_t m_ActiveAnimation;
		float m_AnimationFrame;

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
	};
}