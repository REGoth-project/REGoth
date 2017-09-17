//
// Created by andre on 30.05.16.
//

#include "ContentLoad.h"
#include <engine/World.h>
#include <components/EntityActions.h>

Handle::EntityHandle Content::Wrap::createEntity(World::WorldInstance& world, Components::ComponentMask mask)
{
    return world.addEntity(mask);
}

Components::StaticMeshComponent& Content::Wrap::getStaticMeshComponent(World::WorldInstance& world,
                                                                       Handle::EntityHandle e)
{
    return world.getEntity<Components::StaticMeshComponent>(e);
}

Components::CompoundComponent& Content::Wrap::getCompoundComponent(World::WorldInstance& world,
                                                                   Handle::EntityHandle e)
{
    // Make sure the component is enabled
    Components::Actions::initComponent<Components::CompoundComponent>(world.getComponentAllocator(), e);

    return world.getEntity<Components::CompoundComponent>(e);
}

Handle::TextureHandle Content::Wrap::loadTextureVDF(World::WorldInstance& world, const std::string& file)
{
    return world.getTextureAllocator().loadTextureVDF(file);
}