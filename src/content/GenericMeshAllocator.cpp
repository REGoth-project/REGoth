//
// Created by andre on 09.06.16.
//

#include "GenericMeshAllocator.h"
#include <vdfs/fileIndex.h>
#include <zenload/zCModelMeshLib.h>
#include <zenload/zCMorphMesh.h>
#include <zenload/zCProgMeshProto.h>

using namespace Meshes;

GenericMeshAllocator::GenericMeshAllocator(const VDFS::FileIndex* vdfidx)
    : m_pVDFSIndex(vdfidx)
{
}

Handle::MeshHandle GenericMeshAllocator::loadMeshVDF(const VDFS::FileIndex& idx, const std::string& name)
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
        if (vname.find("3DS") != std::string::npos)
        {
            // Strip the ".3DS"
            vname = vname.substr(0, vname.size() - 4);

            // Add "compiled"-extension
            vname += ".MRM";
        }
        else if (vname.find(".MMS") != std::string::npos)
        {
            // Strip the ".MMS"
            vname = vname.substr(0, vname.size() - 4);

            // Add "compiled"-extension
            vname += ".MMB";
        }
    }

    ZenLoad::PackedMesh packed;

    if (vname.find(".MRM") != std::string::npos)
    {
        // Try to load the mesh
        ZenLoad::zCProgMeshProto zmsh(vname, *m_pVDFSIndex);

        // Failed?
        if (zmsh.getNumSubmeshes() == 0)
            return Handle::MeshHandle::makeInvalidHandle();

        // Pack the mesh
        zmsh.packMesh(packed, 1.0f / 100.0f);
    }
    else if (vname.find(".MMB") != std::string::npos)
    {
        ZenLoad::zCMorphMesh zmm(vname, *m_pVDFSIndex);

        // Failed?
        if (zmm.getMesh().getNumSubmeshes() == 0)
            return Handle::MeshHandle::makeInvalidHandle();

        // Pack the mesh
        zmm.getMesh().packMesh(packed, 1.0f / 100.0f);
    }
    if (vname.find(".MDMS") != std::string::npos)
    {
        vname = vname.substr(0, vname.size() - 1);
        ZenLoad::zCModelMeshLib zlib(vname, *m_pVDFSIndex, 1.0f / 100.0f);

        // Failed?
        if (!zlib.isValid())
            return Handle::MeshHandle::makeInvalidHandle();

        ZenLoad::PackedSkeletalMesh sp;
        zlib.packMesh(sp, 1.0f / 100.0f);

        for (auto& m : zlib.getMeshes())
        {
            m.getMesh().packMesh(packed, 1.0f / 100.0f);
        }
    }

    return loadFromPacked(packed, name);
}

Handle::MeshHandle GenericMeshAllocator::loadMeshVDF(const std::string& name)
{
    if (!m_pVDFSIndex)
        return Handle::MeshHandle::makeInvalidHandle();

    return loadMeshVDF(*m_pVDFSIndex, name);
}
