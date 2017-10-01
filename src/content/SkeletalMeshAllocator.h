#pragma once
#include "GenericMeshAllocator.h"
#include "StaticLevelMesh.h"
#include "memory/Config.h"
#include <handle/Handle.h>
#include <handle/HandleDef.h>
#include <zenload/zCModelMeshLib.h>

namespace Engine
{
    class GameEngine;
}

namespace Meshes
{
    typedef Meshes::SkeletalVertex WorldSkeletalMeshVertex;
    typedef uint32_t WorldSkeletalMeshIndex;
    typedef LevelMesh::StaticLevelMesh<WorldSkeletalMeshVertex, WorldSkeletalMeshIndex> WorldSkeletalMesh;

    class SkeletalMeshAllocator
    {
    public:
        SkeletalMeshAllocator(Engine::GameEngine& engine);
        virtual ~SkeletalMeshAllocator();

        /**
         * @brief Loads a ZTEX-texture from the given or stored VDFS-FileIndex
         */
        Handle::MeshHandle loadMeshVDF(const VDFS::FileIndex& idx, const std::string& name);
        Handle::MeshHandle loadMeshVDF(const std::string& name);

        Handle::MeshHandle loadFromPacked(const ZenLoad::PackedSkeletalMesh& packed, const std::string& name = "");

        /**
         * @brief Returns the texture of the given handle
         */
        WorldSkeletalMesh& getMesh(Handle::MeshHandle h) { return m_Allocator.getElement(h).mesh; }
        bool isLoaded(Handle::MeshHandle h) { return m_Allocator.getElement(h).loaded; }
        const ZenLoad::zCModelMeshLib& getMeshLib(Handle::MeshHandle h) { return m_Allocator.getElement(h).lib; }
        /**
         * @return Rough estimation about how much memory the loaded textures need on the GPU in bytes
         */
        size_t getEstimatedGPUMemoryConsumption() { return m_EstimatedGPUBytes; }
        void getLargestContentInformation(size_t& size, std::string& name)
        {
            size = m_LargestContentBytes;
            name = m_LargestContentName;
        }

    protected:
        /**
         * Pushes the loaded data to the GPU. Needs to run on the main-thread.
         * @param h Data to finalize
         * @return True if successful, false otherwise
         */
        bool finalizeLoad(Handle::MeshHandle h);

        /**
         * Data allocator
         */

        struct SkelMesh : public Handle::HandleTypeDescriptor<Handle::MeshHandle>
        {
            WorldSkeletalMesh mesh;
            ZenLoad::zCModelMeshLib lib;
            bool loaded;
            std::string name;
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
        Engine::GameEngine& m_Engine;

        /**
         * Rough estimation about how much memory the loaded textures need on the GPU
         */
        size_t m_EstimatedGPUBytes = 0;
        size_t m_LargestContentBytes = 0;
        std::string m_LargestContentName;
    };
}
