#pragma once
#include <components/Entities.h>
#include <handle/HandleDef.h>
#include <string>
#include <vector>
#include "StaticLevelMesh.h"

namespace World
{
class WorldInstance;
}

namespace Content
{
/**
     * These exist to allow using world-methods in templated code
     */
namespace Wrap
{
/**
         * @brief Loads the given texture for the given world
         */
Handle::TextureHandle loadTextureVDF(World::WorldInstance& world, const std::string& file);

/**
         * @brief Creates an entity on the given world
         */
Handle::EntityHandle createEntity(World::WorldInstance& world, Components::ComponentMask mask);

/**
         * @brief Returns the <> component of the given entity
         */
Components::StaticMeshComponent& getStaticMeshComponent(World::WorldInstance& world, Handle::EntityHandle e);
Components::CompoundComponent& getCompoundComponent(World::WorldInstance& world, Handle::EntityHandle e);
}

/**
     * @brief Creates entities with components using the submeshes of the given mesh
     * @param world Target-World
     * @param mh Handle of the data passed as "mesh"
     * @param mesh Mesh to take the data from
     * @param compoundTarget If set to a valid handle, the created entities will be put into the compoundComponent
     *                       of that entity
     * @return vector of the created entities
     */
template <typename V, typename I>
std::vector<Handle::EntityHandle> entitifyMesh(World::WorldInstance& world,
                                               Handle::MeshHandle mh,
                                               LevelMesh::StaticLevelMesh<V, I>& mesh,
                                               Handle::EntityHandle compoundTarget = Handle::EntityHandle::makeInvalidHandle())
{
    std::vector<Handle::EntityHandle> r;

    // Create entities for each submesh
    for (size_t i = 0, end = mesh.m_SubmeshStarts.size(); i < end; i++)
    {
        Handle::EntityHandle entity = Wrap::createEntity(world, Components::StaticMeshComponent::MASK);
        Components::StaticMeshComponent& sm = Wrap::getStaticMeshComponent(world, entity);

        // Register mesh and what part to render
        sm.m_StaticMeshVisual = mh;
        sm.m_SubmeshInfo = mesh.m_SubmeshStarts[i];
        sm.m_SubmeshIdx = (uint32_t)i;
        sm.m_Texture = Wrap::loadTextureVDF(world, mesh.m_SubmeshMaterialNames[i]);

        r.push_back(entity);
    }

    // Add to compound
    if (compoundTarget.isValid())
    {
        Components::CompoundComponent& cc = Wrap::getCompoundComponent(world, compoundTarget);
        cc.m_Attachments.insert(cc.m_Attachments.end(), r.begin(), r.end());
    }

    return r;
}
}