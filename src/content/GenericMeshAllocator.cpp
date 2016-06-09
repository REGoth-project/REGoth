//
// Created by andre on 09.06.16.
//

#include "GenericMeshAllocator.h"
#include <vdfs/fileIndex.h>
#include <zenload/zCProgMeshProto.h>
#include <zenload/zCModelMeshLib.h>

using namespace Meshes;

GenericMeshAllocator::GenericMeshAllocator(const VDFS::FileIndex* vdfidx)
        : m_pVDFSIndex(vdfidx)
{
}

GenericMeshAllocator::~GenericMeshAllocator()
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
    }else if(vname.find(".MDM") != std::string::npos || vname.find(".MDL") != std::string::npos)
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

Handle::MeshHandle GenericMeshAllocator::loadMeshVDF(const std::string & name)
{
    if (!m_pVDFSIndex)
        return Handle::MeshHandle::makeInvalidHandle();

    return loadMeshVDF(*m_pVDFSIndex, name);
}
