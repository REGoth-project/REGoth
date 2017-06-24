#include "WorldMesh.h"
#include <zenload/zTypes.h>
#include "World.h"

using namespace World;



WorldMesh::WorldMesh(World::WorldInstance& parentWorld)
	: m_ParentWorld(parentWorld)
{

}

void WorldMesh::load(const ZenLoad::PackedMesh& in)
{
	m_WorldMeshData = in;
	m_BBox3d[0] = Math::float3(in.bbox[0].v);
	m_BBox3d[1] = Math::float3(in.bbox[1].v);
}

float WorldMesh::interpolateTriangleShadowValue(size_t triangleIdx, const Math::float3 &worldPosition)
{
	assert(triangleIdx < m_WorldMeshData.triangles.size());

	ZenLoad::WorldTriangle& tri = m_WorldMeshData.triangles[triangleIdx];

	float u,v,w;
	Math::barycentric(worldPosition, tri.vertices[0].Position.v, tri.vertices[1].Position.v, tri.vertices[2].Position.v, u, v, w);

	Math::float4 c[3];
	c[0].fromABGR8(tri.vertices[0].Color);
	c[1].fromABGR8(tri.vertices[1].Color);
	c[2].fromABGR8(tri.vertices[2].Color);

	return (u * c[0] + v * c[1] + w * c[2]).x; // Lighting is greyscale only
}

void WorldMesh::getTriangle(size_t triangleIdx, Math::float3* v3, uint8_t& matgroup)
{
    assert(triangleIdx < m_WorldMeshData.triangles.size());
    ZenLoad::WorldTriangle& tri = m_WorldMeshData.triangles[triangleIdx];
    if (static_cast<size_t>(tri.submeshIndex) < m_WorldMeshData.subMeshes.size() && tri.submeshIndex >= 0)
        matgroup = m_WorldMeshData.subMeshes[tri.submeshIndex].material.matGroup;
    else
        matgroup = (uint8_t)ZenLoad::MaterialGroup::UNDEF;

    for (int i = 0; i < 3; i++)
        v3[i] = Math::float3(tri.vertices[i].Position.v);
}

ZenLoad::zCMaterialData WorldMesh::getMatData(size_t triangleIdx) const
{
    assert(triangleIdx < m_WorldMeshData.triangles.size());
    assert(m_WorldMeshData.triangles[triangleIdx].submeshIndex < m_WorldMeshData.subMeshes.size());
    return m_WorldMeshData.subMeshes[m_WorldMeshData.triangles[triangleIdx].submeshIndex].material;
}
