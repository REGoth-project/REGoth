#include "SkeletalMeshAllocator.h"
#include <bgfx/bgfx.h>
#include <vdfs/fileIndex.h>
#include <zenload/zCProgMeshProto.h>
#include <zenload/zCModelMeshLib.h>
#include <utils/logger.h>
#include "VertexTypes.h"
#include <engine/BaseEngine.h>

using namespace Meshes;

SkeletalMeshAllocator::SkeletalMeshAllocator(Engine::BaseEngine& engine)
    : m_Engine(engine)
{
}

SkeletalMeshAllocator::~SkeletalMeshAllocator()
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

Handle::MeshHandle SkeletalMeshAllocator::loadMeshVDF(const VDFS::FileIndex& idx, const std::string& name)
{
    // Check if this was already loaded
    auto it = m_MeshesByName.find(name);
    if (it != m_MeshesByName.end())
        return (*it).second;

    std::string vname = name;
    std::vector<uint8_t> data;
    std::vector<uint8_t> dds;

    // Check if this isn't the compiled version
    if (vname.find("-C") == std::string::npos)
    {
        if(vname.find("ASC") != std::string::npos)
        {
            // Strip the ".ASC"
            vname = vname.substr(0, vname.size() - 4);

            // Add "compiled"-extension
            vname += ".MDL";
        } else if(vname.find("MDS") != std::string::npos)
        {
            // Strip the ".MDS"
            vname = vname.substr(0, vname.size() - 4);

            // Add "compiled"-extension
            vname += ".MDL";
        } else if(vname.find(".MDM") != std::string::npos)
        {

        }
    }


    ZenLoad::PackedSkeletalMesh packed;

    if(vname.find(".MDM") != std::string::npos
       || vname.find(".MDL") != std::string::npos
       || vname.find(".MDS") != std::string::npos)
    {
        ZenLoad::zCModelMeshLib zlib(vname, m_Engine.getVDFSIndex(), 1.0f / 100.0f);

        // Failed?
        if (!zlib.isValid())
            return Handle::MeshHandle::makeInvalidHandle();

        ZenLoad::PackedSkeletalMesh sp;
        zlib.packMesh(sp, 1.0f / 100.0f);

        for(auto& m : zlib.getMeshes())
        {
            m.packMesh(packed, 1.0f / 100.0f);
        }

        Handle::MeshHandle h = loadFromPacked(packed, name);
        m_Allocator.getElement(h).lib = zlib;

		// Load failed somehow, cleanup
		if(m_Allocator.getElement(h).mesh.m_VertexBufferHandle.idx == bgfx::invalidHandle
			|| m_Allocator.getElement(h).mesh.m_IndexBufferHandle.idx == bgfx::invalidHandle)
		{
			if(m_Allocator.getElement(h).mesh.m_VertexBufferHandle.idx != bgfx::invalidHandle)
				bgfx::destroyVertexBuffer(m_Allocator.getElement(h).mesh.m_VertexBufferHandle);

			if(m_Allocator.getElement(h).mesh.m_IndexBufferHandle.idx != bgfx::invalidHandle)
				bgfx::destroyIndexBuffer(m_Allocator.getElement(h).mesh.m_IndexBufferHandle);

			m_Allocator.removeObject(h);

			return Handle::MeshHandle::makeInvalidHandle();
		}

        return h;
    }

    return Handle::MeshHandle::makeInvalidHandle();
}

Handle::MeshHandle SkeletalMeshAllocator::loadMeshVDF(const std::string & name)
{
    return loadMeshVDF(m_Engine.getVDFSIndex(), name);
}


Handle::MeshHandle SkeletalMeshAllocator::loadFromPacked(const ZenLoad::PackedSkeletalMesh& packed, const std::string& name)
{
    // Create mesh instance
    Handle::MeshHandle h = m_Allocator.createObject();
    WorldSkeletalMesh& mesh = m_Allocator.getElement(h).mesh;
    m_Allocator.getElement(h).loaded = false;

    // Copy vertices
    mesh.m_Vertices.resize(packed.vertices.size());
    for (size_t i = 0, end = packed.vertices.size(); i < end; i++)
    {
        mesh.m_Vertices[i].TexCoord = Math::float2(packed.vertices[i].TexCoord.x, packed.vertices[i].TexCoord.y);
        mesh.m_Vertices[i].Color = packed.vertices[i].Color;
        mesh.m_Vertices[i].Normal = Math::float3(packed.vertices[i].Normal.x, packed.vertices[i].Normal.y, packed.vertices[i].Normal.z);
        memcpy(mesh.m_Vertices[i].weights, packed.vertices[i].Weights, sizeof(float) * 4);
        memcpy(mesh.m_Vertices[i].boneIndices, packed.vertices[i].BoneIndices, sizeof(char) * 4);

        mesh.m_Vertices[i].Color = 0xFFFFFFFF;

        for(size_t j=0;j<4;j++)
            mesh.m_Vertices[i].localPositions[j] = Math::float3(packed.vertices[i].LocalPositions[j].x,
                                                                packed.vertices[i].LocalPositions[j].y,
                                                                packed.vertices[i].LocalPositions[j].z);
    }

    // Put all indices into one continuous chunk of memory
    for (size_t i = 0, end = packed.subMeshes.size(); i < end; i++)
    {
        auto& m = packed.subMeshes[i];

        mesh.m_SubmeshStarts.push_back({ static_cast<WorldSkeletalMeshIndex>(mesh.m_Indices.size()),
                                         static_cast<WorldSkeletalMeshIndex>(m.indices.size()) });

        mesh.m_Indices.insert(mesh.m_Indices.end(), m.indices.begin(), m.indices.end());

        mesh.m_SubmeshMaterials.emplace_back();
        mesh.m_SubmeshMaterials.back().m_TextureName = m.material.texture;
        mesh.m_SubmeshMaterials.back().m_NoCollision = m.material.noCollDet != 0;
        mesh.m_SubmeshMaterialNames.push_back(m.material.texture);

    }

    // Flush the pipeline to prevent an overflow
    //bgfx::frame();
    m_Engine.onMessage([=](Engine::BaseEngine* pEngine) -> bool {
        finalizeLoad(h);

        return true;
    });

    if(!name.empty())
        m_MeshesByName[name] = h;

    return h;
}


bool SkeletalMeshAllocator::finalizeLoad(Handle::MeshHandle h)
{
    WorldSkeletalMesh& mesh = m_Allocator.getElement(h).mesh;

    // Construct BGFX Vertex/Index-buffers
    mesh.m_VertexBufferHandle = bgfx::createVertexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(mesh.m_Vertices.data(), mesh.m_Vertices.size() * sizeof(WorldSkeletalMeshVertex)),
            WorldSkeletalMeshVertex::ms_decl
    );

    mesh.m_IndexBufferHandle = bgfx::createIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(mesh.m_Indices.data(), mesh.m_Indices.size() * sizeof(WorldSkeletalMeshIndex)),
            sizeof(WorldSkeletalMeshIndex) == 4 ? BGFX_BUFFER_INDEX32 : 0
    );

    m_Allocator.getElement(h).loaded = true;
    return bgfx::isValid(mesh.m_VertexBufferHandle) && bgfx::isValid(mesh.m_IndexBufferHandle);
}