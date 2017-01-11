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

		/**
		 * Interpolates the color on of the vertices on the triangle with the given index
		 * @param triangleIdx Triangle to use for the calculation
		 * @param worldPosition Position to interpolate to (in world-coords)
		 * @return Interpolated color-value on the given triangle
		 */
		float interpolateTriangleShadowValue(size_t triangleIdx, const Math::float3& worldPosition) const;

		/**
		 * Returns the vertices of the given triangle
		 * @param triangleIdx Triangle to get the vertices from
		 * @param v3 pointer to array of 3 Math::float3s
		 * @param matgroup bitmap holding material group info for this triangle
		 */
		void getTriangle(size_t triangleIdx, Math::float3 *v3, uint8_t& matgroup) const;
		void getTriangle(size_t triangleIdx, WorldMeshVertex* v3, uint8_t& matgroup) const;

        /**
         * @return Iterable list of triangles
         */
        const std::vector<ZenLoad::WorldTriangle>& getTriangles() const { return m_WorldMeshData.triangles; };

		/**
		 * @return Boundingbox max/min
		 */
		const Math::float3& getBBoxMin() const { return m_BBox3d[0]; }
		const Math::float3& getBBoxMax() const { return m_BBox3d[1]; }

		/**
		 * Debugging purposes only
		 */
		const ZenLoad::zCMaterialData& getMatData(size_t triangleIdx) const;

        /**
         * Looks up which material is used by the given submesh
         * @param submeshIdx Submesh to look at
         * @return Texture used by the given submesh
         */
        Handle::TextureHandle getSubmeshMatData(size_t submeshIdx) const; // FIXME: Should return material later
	protected:

		/**
		 * Data of the worldmesh
		 */
		ZenLoad::PackedMesh m_WorldMeshData;

        /**
         * List of materials used by submeshes
         */
        std::vector<Handle::TextureHandle> m_WorldMeshTextures;

		/**
		 * Reference to the parentworld
		 */
		World::WorldInstance& m_ParentWorld;

		/**
		 * Total boundingbox of this worldmesh
		 */
		Math::float3 m_BBox3d[2];
	};
}
