#pragma once
#include <handle/Handle.h>
#include <handle/HandleDef.h>
#include <map>
#include <vector>
#include <string>
#include "memory/Config.h"
#include "StaticLevelMesh.h"
#include "GenericMeshAllocator.h"

namespace Engine
{
    class BaseEngine;
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
        bool loaded;
    };

    class StaticMeshAllocator : public GenericMeshAllocator
    {
    public:
        StaticMeshAllocator(Engine::BaseEngine& engine);
        virtual ~StaticMeshAllocator();

        /**
         * Allocates a mesh-instance and returns the handle to it.
         * @param name Name of the mesh-instance to allocate
         * @return Handle to a clean mesh-instance
         */
        Handle::MeshHandle reserveHandle(const std::string& name = "");

        /**
         * Puts all the loaded data into the target mesh
         * @param packed Packed mesh data from
         * @param name Name to help caching
         * @param Whether the vertices are already triangles and we do not need to use an index buffer
         * @return Handle to the mesh from this allocator
         */
        Handle::MeshHandle loadFromPackedTriList(const ZenLoad::PackedMesh& packed, const std::string& name = "", bool triangles = false);
        Handle::MeshHandle loadFromPacked(const ZenLoad::PackedMesh& packed, const std::string& name = "") override {return loadFromPackedTriList(packed, name, false);}
        
        Handle::MeshHandle loadFromPackedSubmesh(const ZenLoad::PackedMesh& packed, size_t submesh, const std::string& name = "");

        /**
         * Pushes the loaded data to the GPU. Needs to run on the main-thread.
         * @param h Data to finalize
         * @return True if successful, false otherwise
         */
        bool finalizeLoad(Handle::MeshHandle h);

        /**
         * @brief Returns the texture of the given handle
         */
        WorldStaticMesh& getMesh(Handle::MeshHandle h) { return m_Allocator.getElement(h); }
    protected:

        /**
         * Data allocator
         */
        Memory::StaticReferencedAllocator<WorldStaticMesh, Config::MAX_NUM_LEVEL_MESHES> m_Allocator;

        /**
         * Engine
         */
        Engine::BaseEngine& m_Engine;
    };

}
