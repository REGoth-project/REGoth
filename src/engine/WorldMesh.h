#pragma once
#include <engine/WorldTypes.h>
#include <content/StaticLevelMesh.h>

namespace ZenLoad
{
	struct PackedMesh;
}

namespace World
{
	class WorldInstance;
	typedef LevelMesh::StaticLevelMesh<WorldMeshVertex, WorldMeshIndex> WorldMeshData;
	
	class WorldMesh
	{
	public:
		WorldMesh(World::WorldInstance& parentWorld);

		/**
		 * @brief Loads the worldmesh from the given Packed-Mesh
		 */
		void load(const ZenLoad::PackedMesh& in);

	protected:

		/**
		 * Data of the whole mesh
		 */
		Handle::MeshHandle m_MeshHandle;

		/**
		 * Handles the worldmeshes submesh-entities
		 */
		std::vector<Handle::EntityHandle> m_MeshEntities;

		/**
		 * Reference to the parentworld
		 */
		World::WorldInstance& m_ParentWorld;
	};
}