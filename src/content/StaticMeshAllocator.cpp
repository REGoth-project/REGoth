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
        bgfx::VertexBufferHandle hv = m_Allocator.getElements()[i].m_VertexBufferHandle;
        bgfx::destroyVertexBuffer(hv);

        bgfx::IndexBufferHandle hi = m_Allocator.getElements()[i].m_IndexBufferHandle;
        bgfx::destroyIndexBuffer(hi);
    }
}

Handle::MeshHandle StaticMeshAllocator::loadFromPacked(const ZenLoad::PackedMesh& packed, const std::string& name)
{
    // Create mesh instance
    Handle::MeshHandle h = m_Allocator.createObject();
    WorldStaticMesh& mesh = m_Allocator.getElement(h);

    // Copy vertices
    mesh.m_Vertices.resize(packed.vertices.size());
    for (size_t i = 0, end = packed.vertices.size(); i < end; i++)
        mesh.m_Vertices[i] = Meshes::vertexCast<WorldStaticMeshVertex>(packed.vertices[i]);


    // Put all indices into one continuous chunk of memory
    for (size_t i = 0, end = packed.subMeshes.size(); i < end; i++)
    {
        auto& m = packed.subMeshes[i];

        mesh.m_SubmeshStarts.push_back({ static_cast<WorldStaticMeshIndex>(mesh.m_Indices.size()),
                                         static_cast<WorldStaticMeshIndex>(m.indices.size()) });

        mesh.m_Indices.insert(mesh.m_Indices.end(), m.indices.begin(), m.indices.end());

        // TODO: Implement materials
        Handle::MaterialHandle h;
        h.invalidate();
        mesh.m_SubmeshMaterials.push_back(h);
        mesh.m_SubmeshMaterialNames.push_back(m.material.texture);

    }

    // Construct BGFX Vertex/Index-buffers
    mesh.m_VertexBufferHandle = bgfx::createVertexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(mesh.m_Vertices.data(), mesh.m_Vertices.size() * sizeof(WorldStaticMeshVertex)),
            WorldStaticMeshVertex::ms_decl
    );

    mesh.m_IndexBufferHandle = bgfx::createIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(mesh.m_Indices.data(), mesh.m_Indices.size() * sizeof(WorldStaticMeshIndex)),
            sizeof(WorldStaticMeshIndex) == 4 ? BGFX_BUFFER_INDEX32 : 0
    );

    // Flush the pipeline to prevent an overflow
    //bgfx::frame();

    if(!name.empty())
        m_MeshesByName[name] = h;

    return h;
}


