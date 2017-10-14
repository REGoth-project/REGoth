#pragma once
#include <content/AnimationAllocator.h>
#include <content/SkeletalMeshAllocator.h>
#include <content/StaticMeshAllocator.h>
#include <content/Texture.h>
#include <content/VertexTypes.h>
#include <memory/StaticReferencedAllocator.h>
#include <memory/AllocatorBundle.h>
#include <memory/Config.h>

namespace World
{
    struct WorldAllocators
    {
        WorldAllocators(Engine::BaseEngine& engine)
                : m_LevelTextureAllocator(engine)
                , m_LevelSkeletalMeshAllocator(engine)
                , m_LevelStaticMeshAllocator(engine)
        {
        }
        template <typename V, typename I>
        using MeshAllocator = Memory::StaticReferencedAllocator<
                Meshes::WorldStaticMesh,
                Config::MAX_NUM_LEVEL_MESHES>;

        Components::ComponentAllocator m_ComponentAllocator;
        Textures::TextureAllocator m_LevelTextureAllocator;
        Meshes::StaticMeshAllocator m_LevelStaticMeshAllocator;
        Meshes::SkeletalMeshAllocator m_LevelSkeletalMeshAllocator;
        Animations::AnimationAllocator m_AnimationAllocator;
        Animations::AnimationDataAllocator m_AnimationDataAllocator;
    };
}