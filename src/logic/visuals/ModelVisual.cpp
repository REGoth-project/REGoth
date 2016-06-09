#include "ModelVisual.h"
#include <engine/World.h>
#include <content/ContentLoad.h>
#include <utils/logger.h>
#include <content/SkeletalMeshAllocator.h>

using namespace Logic;

ModelVisual::ModelVisual(World::WorldInstance& world, Handle::EntityHandle entity)
        : VisualController(world, entity)
{

}

ModelVisual::~ModelVisual()
{

}

bool ModelVisual::load(const std::string& visual)
{
    assert(visual.find(".MDM") != std::string::npos || visual.find(".MDL") != std::string::npos);

    Handle::MeshHandle msh = m_World.getSkeletalMeshAllocator().loadMeshVDF(visual);

    if(!msh.isValid())
        return false;

    Meshes::WorldSkeletalMesh& mdata = m_World.getSkeletalMeshAllocator().getMesh(msh);

    // TODO: Put these into a compound-component or something
    m_VisualEntities = Content::entitifyMesh(m_World, msh, mdata);

    for(Handle::EntityHandle e : m_VisualEntities)
    {
        // Init positions
        Components::EntityComponent& entity = m_World.getEntity<Components::EntityComponent>(e);
        Components::addComponent<Components::PositionComponent>(entity);

        // Copy world-matrix
        Components::PositionComponent& pos = m_World.getEntity<Components::PositionComponent>(e);
        pos.m_WorldMatrix = getEntityTransform();

        // Init animation components
        Components::addComponent<Components::AnimationComponent>(entity);
        Components::AnimationComponent& anim = m_World.getEntity<Components::AnimationComponent>(e);

        // Assign the main-vob as animation controller
        anim.m_ParentAnimHandler = m_Entity;
    }

    Handle::EntityHandle e = m_World.addEntity(Components::BBoxComponent::MASK);
    m_World.getEntity<Components::BBoxComponent>(e).m_DebugColor = 0xFF00FFFF;

    return true;
}

void ModelVisual::setHeadMesh(const std::string& head)
{

}

void ModelVisual::rebuildMainEntityList()
{
    m_VisualEntities.clear();

    m_VisualEntities.insert(m_VisualEntities.end(), m_PartEntities.mainSkelMeshEntities.begin(), m_PartEntities.mainSkelMeshEntities.end());
    m_VisualEntities.insert(m_VisualEntities.end(), m_PartEntities.headMeshEntities.begin(), m_PartEntities.headMeshEntities.end());
    m_VisualEntities.insert(m_VisualEntities.end(), m_PartEntities.armorEntities.begin(), m_PartEntities.armorEntities.end());
}
