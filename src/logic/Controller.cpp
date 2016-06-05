//
// Created by andre on 01.06.16.
//

#include "Controller.h"
#include <engine/World.h>
#include <components/Vob.h>

Logic::Controller::Controller(World::WorldInstance& world, Handle::EntityHandle entity)
    : m_Entity(entity),
      m_World(world)
{

}

void Logic::Controller::setEntityTransform(const Math::Matrix& transform)
{
    Vob::VobInformation vob = Vob::asVob(m_World,m_Entity);
    Vob::setTransform(vob, transform);
}

Math::Matrix& Logic::Controller::getEntityTransform()
{
    return m_World.getEntity<Components::PositionComponent>(m_Entity).m_WorldMatrix;
}