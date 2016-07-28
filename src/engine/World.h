#pragma once

#include <memory/AllocatorBundle.h>
#include <memory/Config.h>
#include <content/Texture.h>
#include <components/Entities.h>
#include <memory/StaticReferencedAllocator.h>
#include <content/VertexTypes.h>
#include "WorldMesh.h"
#include <content/StaticMeshAllocator.h>
#include "Waynet.h"
#include <logic/ScriptEngine.h>
#include <content/SkeletalMeshAllocator.h>
#include <components/Entities.h>
#include <physics/PhysicsSystem.h>
#include <content/AnimationAllocator.h>

namespace ZenLoad
{
	class ZenParser;
}

namespace Engine
{
	class BaseEngine;
}

namespace World
{
    struct WorldAllocators
    {
		template<typename V, typename I>
		using MeshAllocator = Memory::StaticReferencedAllocator<
			LevelMesh::StaticLevelMesh<V,I>, 
			Config::MAX_NUM_LEVEL_MESHES
		>;

		typedef MeshAllocator<Meshes::UVNormColorVertex, uint32_t> WorldMeshAllocator;
		typedef Memory::StaticReferencedAllocator<
				Materials::TexturedMaterial,
				Config::MAX_NUM_LEVEL_MATERIALS> MaterialAllocator;

        Components::ComponentAllocator m_ComponentAllocator;
        Textures::TextureAllocator m_LevelTextureAllocator;
		Meshes::StaticMeshAllocator m_LevelStaticMeshAllocator;
		Meshes::SkeletalMeshAllocator m_LevelSkeletalMeshAllocator;
		Animations::AnimationAllocator m_AnimationAllocator;

		// TODO: Refractor this one into StaticMeshAllocator
		WorldMeshAllocator m_WorldMeshAllocator;
		MaterialAllocator m_MaterialAllocator;
    };

    /**
     * All information inside this struct are only valid at a certain time of the frame, where no entities can be
     * (de)allocated or moved around in any way. The indices stored inside the vectors are a direct mapping to the
     * Elements inside the allocator, which are to be seen as invalid in the next frame!
     */
    struct TransientEntityFeatures
    {
        std::vector<size_t> m_VisibleEntities;
    };

    class WorldInstance : public Handle::HandleTypeDescriptor<Handle::WorldHandle>
    {
    public:

		WorldInstance();

		/**
		 * @brief initialization
		 */
		void init(Engine::BaseEngine& engine);

		/**
		* @param zen file
		*/
		void init(Engine::BaseEngine& engine, const std::string& zen);

        /**
         * Creates an entity with the given components and returns its handle
         */
        Components::ComponentAllocator::Handle addEntity(Components::ComponentMask components = 0);

		/**
		 * Removes the entitiy with the given handle
		 */
		void removeEntity(Handle::EntityHandle h);

        /**
         * Updates this world instances entities
         */
        void onFrameUpdate(double deltaTime, float updateRangeSquared, const Math::Matrix& cameraWorld);

		/**
		 * @return The component associated with the given handle
		 */
		template<typename C>
		C& getEntity(Handle::EntityHandle h)
		{
			return m_Allocators.m_ComponentAllocator.getElement<C>(h);
		}

        /**
         * @return The vob-entity of a vob using the given name
         */
        Handle::EntityHandle getVobEntityByName(const std::string& name)
        {
            auto it = m_VobsByNames.find(name);
            if(it == m_VobsByNames.end())
                return Handle::EntityHandle::makeInvalidHandle();

            return (*it).second;
        }

        /**
         * Goes through the list of all vobs and returns a list of the startwaypoint-indices
         * @return List indices of the waypoints which are used as starting-position
         */
        std::vector<size_t> findStartPoints();

        /**
         * Data access
         */
        Components::ComponentAllocator::DataBundle getComponentDataBundle()
        {
            return m_Allocators.m_ComponentAllocator.getDataBundle();
        }
		WorldAllocators& getAllocators()
		{
			return m_Allocators;
		}
		WorldAllocators::WorldMeshAllocator& getWorldMeshAllocator()
		{
			return m_Allocators.m_WorldMeshAllocator;
		}
		Textures::TextureAllocator& getTextureAllocator()
		{
			return m_Allocators.m_LevelTextureAllocator;
		}
		Components::ComponentAllocator& getComponentAllocator()
		{
			return m_Allocators.m_ComponentAllocator;
		}
		Meshes::StaticMeshAllocator& getStaticMeshAllocator()
		{
			return m_Allocators.m_LevelStaticMeshAllocator;
		}
		Meshes::SkeletalMeshAllocator& getSkeletalMeshAllocator()
		{
			return m_Allocators.m_LevelSkeletalMeshAllocator;
		}
		Animations::AnimationAllocator& getAnimationAllocator()
		{
			return m_Allocators.m_AnimationAllocator;
		}

		// TODO: Depricated, remove
		WorldAllocators::MaterialAllocator& getMaterialAllocator()
		{
			return m_Allocators.m_MaterialAllocator;
		}
		const Waynet::WaynetInstance& getWaynet()
		{
			return m_Waynet;
		}
		Logic::ScriptEngine& getScriptEngine()
		{
			return m_ScriptEngine;
		}
		Handle::WorldHandle getMyHandle()
		{
			return m_MyHandle;
		}
		void setMyHandle(Handle::WorldHandle handle)
		{
			m_MyHandle = handle;
		}
		Engine::BaseEngine* getEngine()
		{
			return m_pEngine;
		}
        Physics::PhysicsSystem& getPhysicsSystem()
        {
            return m_PhysicsSystem;
        }
        Handle::CollisionShapeHandle getStaticObjectCollisionShape()
        {
            return m_StaticWorldObjectCollsionShape;
        }
		WorldMesh& getWorldMesh()
		{
			return m_WorldMesh;
		}
    protected:

		/**
		 * Initializes the Script-Engine for a ZEN-World.
		 * Will load the .DAT-Files and setup the VM.
		 */
		void initializeScriptEngineForZenWorld(const std::string& worldName);

        WorldAllocators m_Allocators;
        TransientEntityFeatures m_TransientEntityFeatures;

		/**
		 * Worldmesh-data
		 */
		WorldMesh m_WorldMesh;

		/**
		 * Waynet-data
		 */
		Waynet::WaynetInstance m_Waynet;

		/**
		 * Engine-instance
		 */
		Engine::BaseEngine* m_pEngine;

		/**
		 * Scripting-Engine of this world
		 */
		Logic::ScriptEngine m_ScriptEngine;

		/**
		 * This worlds physics system
		 */
		Physics::PhysicsSystem m_PhysicsSystem;

		/**
		 * Static collision-shape for the world
		 */
		Handle::CollisionShapeHandle m_StaticWorldObjectCollsionShape;
		Handle::CollisionShapeHandle m_StaticWorldMeshCollsionShape;
		Handle::PhysicsObjectHandle m_StaticWorldMeshPhysicsObject;
		Handle::PhysicsObjectHandle m_StaticWorldObjectPhysicsObject;

		/**
		 * Handle of this world-instance
		 */
		Handle::WorldHandle m_MyHandle;

        /**
         * Map of vobs by their names (If they have one)
         */
        std::unordered_map<std::string, Handle::EntityHandle> m_VobsByNames;
    };
}