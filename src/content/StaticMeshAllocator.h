#pragma once
#include <handle/Handle.h>
#include <handle/HandleDef.h>
#include <map>
#include <vector>
#include <string>
#include "memory/Config.h"
#include "StaticLevelMesh.h"
#include "GenericMeshAllocator.h"

namespace VDFS
{
    class FileIndex;
}

namespace Meshes
{
    typedef Meshes::UVNormColorVertex WorldStaticMeshVertex;
    typedef uint32_t WorldStaticMeshIndex;
    typedef LevelMesh::StaticLevelMesh<WorldStaticMeshVertex, WorldStaticMeshIndex> WorldStaticMeshData;

    struct WorldStaticMesh : public Handle::HandleTypeDescriptor<Handle::MeshHandle>
    {
        void init()
        {
            instanceDataBufferIndex = (uint32_t)-1;
        }

        WorldStaticMeshData mesh;

        // Slot where the instance-buffer for meshes of these kind is. Needs to be initialized manually.
        uint32_t instanceDataBufferIndex;
    };

    class StaticMeshAllocator : public GenericMeshAllocator
    {
    public:
        StaticMeshAllocator(const VDFS::FileIndex* vdfidx = nullptr);
        virtual ~StaticMeshAllocator();


        /**
         * Puts all the loaded data into the target mesh
         * @param packed Packed mesh data from
         * @param name Name to help caching
         * @return Handle to the mesh from this allocator
         */
        Handle::MeshHandle loadFromPacked(const ZenLoad::PackedMesh& packed, const std::string& name = "") override;

        /**
         * @brief Returns the texture of the given handle
         */
        WorldStaticMesh& getMesh(Handle::MeshHandle h) { return m_Allocator.getElement(h); }
    protected:

        /**
         * Data allocator
         */
        Memory::StaticReferencedAllocator<WorldStaticMesh, Config::MAX_NUM_LEVEL_MESHES> m_Allocator;


    };

}