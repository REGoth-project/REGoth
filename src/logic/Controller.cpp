//
// Created by andre on 01.06.16.
//

#include "Controller.h"
#include <components/Vob.h>
#include <engine/World.h>
#include <json.hpp>

using json = nlohmann::json;

Logic::Controller::Controller(World::WorldInstance& world, Handle::EntityHandle entity)
    : m_Entity(entity),
      m_World(world),
      m_EventManager(world, entity)
{
}

void Logic::Controller::setEntityTransform(const Math::Matrix& transform)
{
    Vob::VobInformation vob = Vob::asVob(m_World, m_Entity);
    Vob::setTransform(vob, transform);
}

Math::Matrix& Logic::Controller::getEntityTransform()
{
    return m_World.getEntity<Components::PositionComponent>(m_Entity).m_WorldMatrix;
}

void Logic::Controller::onUpdate(float deltaTime)
{
}

void Logic::Controller::exportObject(json& j)
{
    exportPart(j);
}

void Logic::Controller::exportPart(json& j)
{
    j["type"] = "Controller";

    // This doesn't work sometimes when directly assigning... null-values end up on instance 7412

    float values[16];

    for (int i = 0; i < 16; i++)
        values[i] = getEntityTransform().mv[i];

    for (int i = 0; i < 16; i++)
        j["transform"].push_back(values[i]);

    Vob::VobInformation vob = Vob::asVob(m_World, m_Entity);

    j["collision"] = vob.object->m_EnableCollision;
    // TODO: EventMessages?
}

void Logic::Controller::importObject(const json& j)
{
    Math::Matrix transform = Math::Matrix::CreateIdentity();

    auto& jtrans = j["transform"];

    // Parse transform from json
    for (int i = 0; i < 16; i++)
        if (!jtrans[i].is_null())
            transform.mv[i] = jtrans[i];

    setEntityTransform(transform);

    Vob::VobInformation vob = Vob::asVob(m_World, m_Entity);
    vob.object->m_EnableCollision = j["collision"];
}
