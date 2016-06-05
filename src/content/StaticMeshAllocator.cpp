#include "StaticMeshAllocator.h"
#include <bgfx/bgfx.h>
#include <vdfs/fileIndex.h>
#include <zenload/zCProgMeshProto.h>
#include <zenload/zCModelMeshLib.h>
#include "VertexTypes.h"

using namespace Meshes;

StaticMeshAllocator::StaticMeshAllocator(const VDFS::FileIndex* vdfidx)
        : m_pVDFSIndex(vdfidx)
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

Handle::MeshHandle StaticMeshAllocator::loadMeshVDF(const VDFS::FileIndex& idx, const std::string& name)
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
        if(vname.find("3DS") != std::string::npos)
        {
            // Strip the ".3DS"
            vname = vname.substr(0, vname.size() - 4);

            // Add "compiled"-extension
            vname += ".MRM";
        } else if(vname.find(".MDM") != std::string::npos)
        {

        }
    }


    ZenLoad::PackedMesh packed;

    if(vname.find(".MRM") != std::string::npos)
    {
        // Try to load the mesh
        ZenLoad::zCProgMeshProto zmsh(vname, *m_pVDFSIndex);

        // Failed?
        if (zmsh.getNumSubmeshes() == 0)
            return Handle::MeshHandle::makeInvalidHandle();

        // Pack the mesh
        zmsh.packMesh(packed, 1.0f / 100.0f);
    }else if(vname.find(".MDM") != std::string::npos)
    {
        ZenLoad::zCModelMeshLib zlib(vname, *m_pVDFSIndex);

        // Failed?
        if (zlib.getMeshes().size() == 0)
            return Handle::MeshHandle::makeInvalidHandle();

        ZenLoad::PackedSkeletalMesh sp;
        zlib.packMesh(sp, 1.0f / 100.0f);

        for(auto& m : zlib.getMeshes())
        {
            m.getMesh().packMesh(packed, 1.0f / 100.0f);
        }
    }

    return loadFromPacked(packed, name);
}

Handle::MeshHandle StaticMeshAllocator::loadMeshVDF(const std::string & name)
{
    if (!m_pVDFSIndex)
        return Handle::MeshHandle::makeInvalidHandle();

    return loadMeshVDF(*m_pVDFSIndex, name);
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


