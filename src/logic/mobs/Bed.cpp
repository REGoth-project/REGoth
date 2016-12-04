//
// Created by desktop on 06.10.16.
//

#include "Bed.h"

using namespace Logic;
using namespace MobCores;

Bed::Bed(World::WorldInstance& world, const Handle::EntityHandle& entity) : MobCore(world, entity)
{
    m_EnteringBackside = false;
}

Bed::~Bed()
{

}

std::string Bed::getSchemeName()
{
    return m_Scheme + "_" + (m_EnteringBackside ? "BACK" : "FRONT");
}

void Bed::onFreePositionFound(Handle::EntityHandle npc, InteractPosition* pos)
{
    if(pos)
    {
        // Get side the npc is entering the bed from
        m_EnteringBackside = pos->nodeName.find("FRONT") == std::string::npos;
    }
}
