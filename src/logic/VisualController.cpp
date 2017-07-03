//
// Created by andre on 02.06.16.
//

#include "VisualController.h"
#include <engine/World.h>
#include <json.hpp>

using json = nlohmann::json;
using namespace Logic;

VisualController::VisualController(World::WorldInstance& world, Handle::EntityHandle entity)
    : Controller(world, entity), m_Transient(false)
{
}

VisualController::~VisualController()
{
    // Remove all created entities from the world
    for (Handle::EntityHandle e : m_VisualEntities)
        m_World.removeEntity(e);
}

void VisualController::onTransformChanged()
{
    Controller::onTransformChanged();

    // Set all created visuals to the same transform as our entity
    for (Handle::EntityHandle e : m_VisualEntities)
        m_World.getEntity<Components::PositionComponent>(e).m_WorldMatrix = getEntityTransform();
}

void VisualController::exportPart(json& j)
{
    Controller::exportPart(j);

    j["type"] = "VisualController";
    j["name"] = m_Name;
}
