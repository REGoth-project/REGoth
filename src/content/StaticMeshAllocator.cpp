#include "StaticMeshAllocator.h"
#include "VertexTypes.h"
#include <bgfx/bgfx.h>
#include <engine/BaseEngine.h>
#include <utils/logger.h>
#include <vdfs/fileIndex.h>
#include <zenload/zCModelMeshLib.h>
#include <zenload/zCProgMeshProto.h>

using namespace Meshes;

StaticMeshAllocator::StaticMeshAllocator(Engine::BaseEngine& engine)
    : GenericMeshAllocator(&engine.getVDFSIndex())
    , m_Engine(engine)
{
}

StaticMeshAllocator::~StaticMeshAllocator()
{
    // Delete all meshes
    for (size_t i = 0; i < m_Allocator.getNumObtainedElements(); i++)
    {
        bgfx::VertexBufferHandle hv = m_Allocator.getElements()[i].mesh.m_VertexBufferHandle;
        if (bgfx::isValid(hv))
            bgfx::destroy(hv);

        bgfx::IndexBufferHandle hi = m_Allocator.getElements()[i].mesh.m_IndexBufferHandle;
        if (bgfx::isValid(hi))
            bgfx::destroy(hi);
    }

    m_EstimatedGPUBytes = 0;
}

Handle::MeshHandle StaticMeshAllocator::loadFromPackedSubmesh(const ZenLoad::PackedMesh& packed, size_t submesh, const std::string& name)
{
    // Create mesh instance
    Handle::MeshHandle h = m_Allocator.createObject();
    WorldStaticMesh& mesh = m_Allocator.getElement(h);
    mesh.loaded = false;
    mesh.name = name;

    auto& m = packed.subMeshes[submesh];
    mesh.init();

    mesh.bBox3D.min = packed.bbox[0].v;
    mesh.bBox3D.max = packed.bbox[1].v;
    mesh.boundingSphereRadius = (mesh.bBox3D.min * 0.5f + mesh.bBox3D.max * 0.5f).length() + (mesh.bBox3D.max - mesh.bBox3D.min).length() * 0.5f;

    mesh.mesh.m_Vertices.resize(m.indices.size());

    for (size_t i = 0; i < m.indices.size(); i++)
    {
        mesh.mesh.m_Vertices[i] = Meshes::vertexCast<WorldStaticMeshVertex>(packed.vertices[m.indices[i]]);
    }

    mesh.mesh.m_SubmeshStarts.push_back({static_cast<WorldStaticMeshIndex>(0),
                                         static_cast<WorldStaticMeshIndex>(mesh.mesh.m_Vertices.size())});

    mesh.mesh.m_SubmeshMaterials.emplace_back();
    mesh.mesh.m_SubmeshMaterials.back().m_TextureName = m.material.texture;
    mesh.mesh.m_SubmeshMaterials.back().m_NoCollision = m.material.noCollDet;
    mesh.mesh.m_SubmeshMaterials.back().m_MatGroup = (ZenLoad::MaterialGroup)m.material.matGroup;
    mesh.mesh.m_SubmeshMaterialNames.push_back(m.material.texture);

    mesh.mesh.m_IndexBufferHandle.idx = bgfx::kInvalidHandle;
    mesh.mesh.m_VertexBufferHandle.idx = bgfx::kInvalidHandle;

    m_Engine.executeInMainThread<void>([this, h](Engine::BaseEngine* pEngine) {
        finalizeLoad(h);
    });

    m_MeshesByName[name] = h;

    return h;
}

Handle::MeshHandle StaticMeshAllocator::loadFromPackedTriList(const ZenLoad::PackedMesh& packed, const std::string& name, bool triangles)
{
    // Create mesh instance
    Handle::MeshHandle h = m_Allocator.createObject();
    WorldStaticMesh& mesh = m_Allocator.getElement(h);
    mesh.loaded = false;
    mesh.name = name;

    mesh.init();

    mesh.bBox3D.min = packed.bbox[0].v;
    mesh.bBox3D.max = packed.bbox[1].v;
    mesh.boundingSphereRadius = (mesh.bBox3D.min * 0.5f + mesh.bBox3D.max * 0.5f).length() + (mesh.bBox3D.max - mesh.bBox3D.min).length() * 0.5f;

    // Copy vertices
    mesh.mesh.m_Vertices.resize(packed.vertices.size());
    for (size_t i = 0, end = packed.vertices.size(); i < end; i++)
        mesh.mesh.m_Vertices[i] = Meshes::vertexCast<WorldStaticMeshVertex>(packed.vertices[i]);

    for (size_t i = 0, end = packed.vertices.size(); i < end; i++)
        mesh.mesh.m_Vertices[i].Position = packed.vertices[i].Position.v;

    if (!triangles)
    {
        // Put all indices into one continuous chunk of memory
        for (size_t i = 0, end = packed.subMeshes.size(); i < end; i++)
        {
            auto& m = packed.subMeshes[i];

            mesh.mesh.m_SubmeshStarts.push_back({static_cast<WorldStaticMeshIndex>(mesh.mesh.m_Indices.size()),
                                                 static_cast<WorldStaticMeshIndex>(m.indices.size())});

            mesh.mesh.m_Indices.insert(mesh.mesh.m_Indices.end(), m.indices.begin(), m.indices.end());

            mesh.mesh.m_SubmeshMaterials.emplace_back();
            mesh.mesh.m_SubmeshMaterials.back().m_TextureName = m.material.texture;
            mesh.mesh.m_SubmeshMaterials.back().m_NoCollision = m.material.noCollDet;
            mesh.mesh.m_SubmeshMaterialNames.push_back(m.material.texture);

            // FIXME: Hack, brighten indoor-vertices
            for (size_t j = 0; j < m.triangleLightmapIndices.size(); j++)
            {
                if (m.triangleLightmapIndices[j] != -1)
                {
                    const uint32_t hackIndoorBrightness = 0xFF888888;
                    mesh.mesh.m_Vertices[mesh.mesh.m_Indices[mesh.mesh.m_SubmeshStarts.back().m_StartIndex + 3 * j + 0]].Color = hackIndoorBrightness;
                    mesh.mesh.m_Vertices[mesh.mesh.m_Indices[mesh.mesh.m_SubmeshStarts.back().m_StartIndex + 3 * j + 1]].Color = hackIndoorBrightness;
                    mesh.mesh.m_Vertices[mesh.mesh.m_Indices[mesh.mesh.m_SubmeshStarts.back().m_StartIndex + 3 * j + 2]].Color = hackIndoorBrightness;
                }
            }
        }
    }
    else
    {
        size_t idxStart = 0;
        for (size_t i = 0, end = packed.subMeshes.size(); i < end; i++)
        {
            auto& m = packed.subMeshes[i];

            mesh.mesh.m_SubmeshMaterials.emplace_back();
            mesh.mesh.m_SubmeshMaterials.back().m_TextureName = m.material.texture;
            mesh.mesh.m_SubmeshMaterials.back().m_NoCollision = m.material.noCollDet;
            mesh.mesh.m_SubmeshMaterialNames.push_back(m.material.texture);

            mesh.mesh.m_SubmeshStarts.push_back({static_cast<WorldStaticMeshIndex>(idxStart),
                                                 static_cast<WorldStaticMeshIndex>(m.indices.size())});
            idxStart += m.indices.size();
        }
    }

    mesh.mesh.m_IndexBufferHandle.idx = bgfx::kInvalidHandle;
    mesh.mesh.m_VertexBufferHandle.idx = bgfx::kInvalidHandle;

    m_Engine.executeInMainThread<void>([this, h](Engine::BaseEngine* pEngine) {
        bgfx::frame();  // Flush the pipeline to prevent an overflow
        finalizeLoad(h);
    });

    m_MeshesByName[name] = h;

    return h;
}

bool StaticMeshAllocator::finalizeLoad(Handle::MeshHandle h)
{
    WorldStaticMesh& mesh = m_Allocator.getElement(h);

    // Construct BGFX Vertex/Index-buffers
    mesh.mesh.m_VertexBufferHandle = bgfx::createVertexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(mesh.mesh.m_Vertices.data(), mesh.mesh.m_Vertices.size() * sizeof(WorldStaticMeshVertex)),
        WorldStaticMeshVertex::ms_decl);

    size_t contentBytes = mesh.mesh.m_Vertices.size() * sizeof(WorldStaticMeshVertex);

    mesh.mesh.m_IndexBufferHandle.idx = bgfx::kInvalidHandle;
    if (!mesh.mesh.m_Indices.empty())
    {
        mesh.mesh.m_IndexBufferHandle = bgfx::createIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(mesh.mesh.m_Indices.data(), mesh.mesh.m_Indices.size() * sizeof(WorldStaticMeshIndex)),
            sizeof(WorldStaticMeshIndex) == 4 ? BGFX_BUFFER_INDEX32 : 0);

        contentBytes += mesh.mesh.m_Indices.size() * sizeof(WorldStaticMeshIndex);
    }

    if (m_LargestContentBytes < contentBytes)
    {
        m_LargestContentBytes = contentBytes;
        m_LargestContentName = m_Allocator.getElement(h).name;
    }

    m_EstimatedGPUBytes += contentBytes;

    mesh.loaded = true;
    return bgfx::isValid(mesh.mesh.m_IndexBufferHandle) && bgfx::isValid(mesh.mesh.m_VertexBufferHandle);
}
