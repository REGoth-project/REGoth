#pragma once
#include <handle/Handle.h>
#include <handle/HandleDef.h>
#include <map>
#include <vector>
#include <string>
#include "memory/Config.h"
#include "StaticLevelMesh.h"

namespace VDFS
{
    class FileIndex;
}

namespace Meshes
{
    typedef Meshes::PositionUVVertex WorldStaticMeshVertex;
    typedef uint32_t WorldStaticMeshIndex;
    typedef LevelMesh::StaticLevelMesh<WorldStaticMeshVertex, WorldStaticMeshIndex> WorldStaticMesh;

    class StaticMeshAllocator
    {
    public:
        StaticMeshAllocator(const VDFS::FileIndex* vdfidx = nullptr);
        virtual ~StaticMeshAllocator();

        /**
         * @brief Sets the VDFS-Index to use
         */
        void setVDFSIndex(const VDFS::FileIndex* vdfidx) { m_pVDFSIndex = vdfidx; }

        /**
         * @brief Loads a ZTEX-texture from the given or stored VDFS-FileIndex
         */
        Handle::MeshHandle loadMeshVDF(const VDFS::FileIndex& idx, const std::string& name);
        Handle::MeshHandle loadMeshVDF(const std::string& name);
        Handle::MeshHandle loadFromPacked(const ZenLoad::PackedMesh& packed, const std::string& name = "");

        /**
         * @brief Returns the texture of the given handle
         */
        WorldStaticMesh& getMesh(Handle::MeshHandle h) { return m_Allocator.getElement(h); }
    protected:
        /**
         * @brief Textures by their set names. Note: If names are doubled, only the last loaded texture
         *		  can be found here
         */
        std::map<std::string, Handle::MeshHandle> m_MeshesByName;

        /**
         * Data allocator
         */
        Memory::StaticReferencedAllocator<WorldStaticMesh, Config::MAX_NUM_LEVEL_MESHES> m_Allocator;

        /**
         * Pointer to a vdfs-index to work on (can be nullptr)
         */
        const VDFS::FileIndex* m_pVDFSIndex;
    };

}