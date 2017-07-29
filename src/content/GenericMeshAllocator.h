#pragma once

#include <handle/HandleDef.h>
#include <map>
#include <vector>
#include <string>

namespace VDFS
{
    class FileIndex;
}

namespace ZenLoad
{
    struct PackedMesh;
}

namespace Meshes
{
    class GenericMeshAllocator
    {
    public:
        GenericMeshAllocator(const VDFS::FileIndex* vdfidx = nullptr);
        virtual ~GenericMeshAllocator();

        /**
        * @brief Sets the VDFS-Index to use
        */
        void setVDFSIndex(const VDFS::FileIndex* vdfidx) { m_pVDFSIndex = vdfidx; }
        /**
         * @brief Loads a ZTEX-texture from the given or stored VDFS-FileIndex
         */
        Handle::MeshHandle loadMeshVDF(const VDFS::FileIndex& idx, const std::string& name);
        Handle::MeshHandle loadMeshVDF(const std::string& name);

        /**
         * Puts all the loaded data into the target mesh
         * @param packed Packed mesh data from
         * @param name Name to help caching
         * @return Handle to the mesh from this allocator
         */
        virtual Handle::MeshHandle loadFromPacked(const ZenLoad::PackedMesh& packed, const std::string& name = "") = 0;

    protected:
        /**
         * @brief Textures by their set names. Note: If names are doubled, only the last loaded texture
         *		  can be found here
         */
        std::map<std::string, Handle::MeshHandle> m_MeshesByName;

        /**
         * Pointer to a vdfs-index to work on (can be nullptr)
         */
        const VDFS::FileIndex* m_pVDFSIndex;
    };
}