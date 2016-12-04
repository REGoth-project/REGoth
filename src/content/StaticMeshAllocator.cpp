#include "StaticMeshAllocator.h"
#include <bgfx/bgfx.h>
#include <vdfs/fileIndex.h>
#include <zenload/zCProgMeshProto.h>
#include <zenload/zCModelMeshLib.h>
#include <utils/logger.h>
#include "VertexTypes.h"

using namespace Meshes;

StaticMeshAllocator::StaticMeshAllocator(const VDFS::FileIndex* vdfidx)
        : GenericMeshAllocator(vdfidx)
{
}

StaticMeshAllocator::~StaticMeshAllocator()
{
    // Delete all meshes
    for (size_t i = 0; i < m_Allocator.getNumObtainedElements(); i++)
    {
        bgfx::VertexBufferHandle hv = m_Allocator.getElements()[i].mesh.m_VertexBufferHandle;
        bgfx::destroyVertexBuffer(hv);

        bgfx::IndexBufferHandle hi = m_Allocator.getElements()[i].mesh.m_IndexBufferHandle;
        bgfx::destroyIndexBuffer(hi);
    }
}

Handle::MeshHandle StaticMeshAllocator::loadFromPacked(const ZenLoad::PackedMesh& packed, const std::string& name)
{
    // Create mesh instance
    Handle::MeshHandle h = m_Allocator.createObject();
    WorldStaticMesh& mesh = m_Allocator.getElement(h);

    mesh.init();

    // Copy vertices
    mesh.mesh.m_Vertices.resize(packed.vertices.size());
    for (size_t i = 0, end = packed.vertices.size(); i < end; i++)
        mesh.mesh.m_Vertices[i] = Meshes::vertexCast<WorldStaticMeshVertex>(packed.vertices[i]);


    // Put all indices into one continuous chunk of memory
    for (size_t i = 0, end = packed.subMeshes.size(); i < end; i++)
    {
        auto& m = packed.subMeshes[i];

        mesh.mesh.m_SubmeshStarts.push_back({ static_cast<WorldStaticMeshIndex>(mesh.mesh.m_Indices.size()),
                                         static_cast<WorldStaticMeshIndex>(m.indices.size()) });

        mesh.mesh.m_Indices.insert(mesh.mesh.m_Indices.end(), m.indices.begin(), m.indices.end());

        mesh.mesh.m_SubmeshMaterials.emplace_back();
        mesh.mesh.m_SubmeshMaterials.back().m_TextureName = m.material.texture;
        mesh.mesh.m_SubmeshMaterials.back().m_NoCollision = m.material.noCollDet;
        mesh.mesh.m_SubmeshMaterialNames.push_back(m.material.texture);

        // FIXME: Hack, brighten indoor-vertices
        for(size_t j=0;j<m.triangleLightmapIndices.size();j++)
        {
            if(m.triangleLightmapIndices[j] != -1)
            {
                const uint32_t hackIndoorBrightness = 0xFF888888;
                mesh.mesh.m_Vertices[mesh.mesh.m_Indices[mesh.mesh.m_SubmeshStarts.back().m_StartIndex + 3*j + 0]].Color = hackIndoorBrightness;
                mesh.mesh.m_Vertices[mesh.mesh.m_Indices[mesh.mesh.m_SubmeshStarts.back().m_StartIndex + 3*j + 1]].Color = hackIndoorBrightness;
                mesh.mesh.m_Vertices[mesh.mesh.m_Indices[mesh.mesh.m_SubmeshStarts.back().m_StartIndex + 3*j + 2]].Color = hackIndoorBrightness;
            }
        }
    }

    // Construct BGFX Vertex/Index-buffers
    mesh.mesh.m_VertexBufferHandle = bgfx::createVertexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(mesh.mesh.m_Vertices.data(), mesh.mesh.m_Vertices.size() * sizeof(WorldStaticMeshVertex)),
            WorldStaticMeshVertex::ms_decl
    );

    mesh.mesh.m_IndexBufferHandle = bgfx::createIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(mesh.mesh.m_Indices.data(), mesh.mesh.m_Indices.size() * sizeof(WorldStaticMeshIndex)),
            sizeof(WorldStaticMeshIndex) == 4 ? BGFX_BUFFER_INDEX32 : 0
    );

    // Flush the pipeline to prevent an overflow
    //bgfx::frame();


	// Load failed somehow, cleanup
	if(mesh.mesh.m_VertexBufferHandle.idx == bgfx::invalidHandle
		|| mesh.mesh.m_IndexBufferHandle.idx == bgfx::invalidHandle)
	{
		m_Allocator.removeObject(h);

		if(mesh.mesh.m_VertexBufferHandle.idx != bgfx::invalidHandle)
			bgfx::destroyVertexBuffer(mesh.mesh.m_VertexBufferHandle);

		if(mesh.mesh.m_IndexBufferHandle.idx != bgfx::invalidHandle)
			bgfx::destroyIndexBuffer(mesh.mesh.m_IndexBufferHandle);

		return Handle::MeshHandle::makeInvalidHandle();
	}

	m_MeshesByName[name] = h;

    return h;
}


