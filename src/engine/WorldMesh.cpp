#include "WorldMesh.h"
#include <zenload/zTypes.h>
#include "World.h"

using namespace World;



WorldMesh::WorldMesh(World::WorldInstance & parentWorld)
	: m_ParentWorld(parentWorld)
{

}

void WorldMesh::load(const ZenLoad::PackedMesh & in)
{
	// FIXME: There is almost identical code inside StaticMeshAllocator!
	// FIXME: Refractor to use LevelMesh::entitfyMesh!

	m_MeshHandle = m_ParentWorld.getAllocators().m_WorldMeshAllocator.createObject();
	WorldMeshData& mesh = m_ParentWorld.getAllocators().m_WorldMeshAllocator.getElement(m_MeshHandle);

	// Copy vertices
	mesh.m_Vertices.resize(in.vertices.size());
	for (size_t i = 0, end = in.vertices.size(); i < end; i++)
		mesh.m_Vertices[i] = Meshes::vertexCast<WorldMeshVertex>(in.vertices[i]);


	// Put all indices into one continuous chunk of memory
	for (size_t i = 0, end = in.subMeshes.size(); i < end; i++)
	{
		auto& m = in.subMeshes[i];

		mesh.m_SubmeshStarts.push_back({ static_cast<WorldMeshIndex>(mesh.m_Indices.size()),
										 static_cast<WorldMeshIndex>(m.indices.size()) });

		mesh.m_Indices.insert(mesh.m_Indices.end(), m.indices.begin(), m.indices.end());

		// TODO: Implement materials
		Handle::MaterialHandle h;
		h.invalidate();
		mesh.m_SubmeshMaterials.push_back(h);
	}

	// Construct BGFX Vertex/Index-buffers
	mesh.m_VertexBufferHandle = bgfx::createVertexBuffer(
		// Static data can be passed with bgfx::makeRef
		bgfx::makeRef(mesh.m_Vertices.data(), mesh.m_Vertices.size() * sizeof(WorldMeshVertex)),
		WorldMeshVertex::ms_decl
		);

	mesh.m_IndexBufferHandle = bgfx::createIndexBuffer(
		// Static data can be passed with bgfx::makeRef
		bgfx::makeRef(mesh.m_Indices.data(), mesh.m_Indices.size() * sizeof(WorldMeshIndex)), 
		sizeof(WorldMeshIndex) == 4 ? BGFX_BUFFER_INDEX32 : 0
		);

	// Create entities for each submesh
	for (size_t i = 0, end = mesh.m_SubmeshStarts.size(); i < end; i++)
	{
		Handle::EntityHandle entity = m_ParentWorld.addEntity(Components::StaticMeshComponent::MASK);
		Components::StaticMeshComponent& sm = m_ParentWorld.getEntity<Components::StaticMeshComponent>(entity);

		// Register mesh and what part to render
		sm.m_StaticMeshVisual = m_MeshHandle;
		sm.m_SubmeshInfo = mesh.m_SubmeshStarts[i];
		sm.m_Texture = m_ParentWorld.getTextureAllocator().loadTextureVDF(in.subMeshes[i].material.texture);
		// TODO: Register material

		m_MeshEntities.push_back(entity);
	}
}
