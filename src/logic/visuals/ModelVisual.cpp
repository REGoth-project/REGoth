#include "ModelVisual.h"
#include <engine/World.h>
#include <content/ContentLoad.h>

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
    assert(visual.find(".ASC") != std::string::npos);

    /*Handle::MeshHandle msh = m_World.getStaticMeshAllocator().loadMeshVDF(visual);

    if(!msh.isValid())
        return false;

    Meshes::WorldStaticMesh& mdata = m_World.getStaticMeshAllocator().getMesh(msh);

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
    }
**/
    Handle::EntityHandle e = m_World.addEntity(Components::BBoxComponent::MASK);
    m_World.getEntity<Components::BBoxComponent>(e).m_DebugColor = 0xFF00FFFF;

    return true;
}
