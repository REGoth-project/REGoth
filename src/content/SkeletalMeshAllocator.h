#pragma once
#include <handle/Handle.h>
#include <handle/HandleDef.h>
#include <zenload/zCModelMeshLib.h>
#include "memory/Config.h"
#include "StaticLevelMesh.h"
#include "GenericMeshAllocator.h"

namespace Engine
{
    class BaseEngine;
}

namespace Meshes
{
    typedef Meshes::SkeletalVertex WorldSkeletalMeshVertex;
    typedef uint32_t WorldSkeletalMeshIndex;
    typedef LevelMesh::StaticLevelMesh<WorldSkeletalMeshVertex, WorldSkeletalMeshIndex> WorldSkeletalMesh;

    class SkeletalMeshAllocator
    {
    public:
        SkeletalMeshAllocator(Engine::BaseEngine& engine);
        virtual ~SkeletalMeshAllocator();

        /**
         * @brief Loads a ZTEX-texture from the given or stored VDFS-FileIndex
         */
        Handle::MeshHandle loadMeshVDF(const VDFS::FileIndex& idx, const std::string& name);
        Handle::MeshHandle loadMeshVDF(const std::string& name);

        Handle::MeshHandle loadFromPacked(const ZenLoad::PackedSkeletalMesh& packed, const std::string& name = "");

        /**
         * Pushes the loaded data to the GPU. Needs to run on the main-thread.
         * @param h Data to finalize
         * @return True if successful, false otherwise
         */
        bool finalizeLoad(Handle::MeshHandle h);

        /**
         * @brief Returns the texture of the given handle
         */
        WorldSkeletalMesh& getMesh(Handle::MeshHandle h) { return m_Allocator.getElement(h).mesh; }
        bool isLoaded(Handle::MeshHandle h){ return m_Allocator.getElement(h).loaded; }
        const ZenLoad::zCModelMeshLib& getMeshLib(Handle::MeshHandle h){ return m_Allocator.getElement(h).lib; }
    protected:

        /**
         * Data allocator
         */

        struct SkelMesh : public Handle::HandleTypeDescriptor<Handle::MeshHandle>
        {
            WorldSkeletalMesh mesh;
            ZenLoad::zCModelMeshLib lib;
            bool loaded;
        };

        Memory::StaticReferencedAllocator<SkelMesh, Config::MAX_NUM_LEVEL_MESHES> m_Allocator;

        /**
         * @brief Textures by their set names. Note: If names are doubled, only the last loaded texture
         *		  can be found here
         */
        std::map<std::string, Handle::MeshHandle> m_MeshesByName;

        /**
         * Engine
         */
        Engine::BaseEngine& m_Engine;
    };

}