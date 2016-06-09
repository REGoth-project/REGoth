#pragma once
#include <handle/Handle.h>
#include <handle/HandleDef.h>
#include "memory/Config.h"
#include "StaticLevelMesh.h"
#include "GenericMeshAllocator.h"

namespace Meshes
{
    typedef Meshes::SkeletalVertex WorldSkeletalMeshVertex;
    typedef uint32_t WorldSkeletalMeshIndex;
    typedef LevelMesh::StaticLevelMesh<WorldSkeletalMeshVertex, WorldSkeletalMeshIndex> WorldSkeletalMesh;

    class SkeletalMeshAllocator
    {
    public:
        SkeletalMeshAllocator(const VDFS::FileIndex* vdfidx = nullptr);
        virtual ~SkeletalMeshAllocator();

        /**
        * @brief Sets the VDFS-Index to use
        */
        void setVDFSIndex(const VDFS::FileIndex* vdfidx) { m_pVDFSIndex = vdfidx; }

        /**
         * @brief Loads a ZTEX-texture from the given or stored VDFS-FileIndex
         */
        Handle::MeshHandle loadMeshVDF(const VDFS::FileIndex& idx, const std::string& name);
        Handle::MeshHandle loadMeshVDF(const std::string& name);

        Handle::MeshHandle loadFromPacked(const ZenLoad::PackedSkeletalMesh& packed, const std::string& name = "");

        /**
         * @brief Returns the texture of the given handle
         */
        WorldSkeletalMesh& getMesh(Handle::MeshHandle h) { return m_Allocator.getElement(h); }
    protected:

        /**
         * Data allocator
         */
        Memory::StaticReferencedAllocator<WorldSkeletalMesh, Config::MAX_NUM_LEVEL_MESHES> m_Allocator;

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