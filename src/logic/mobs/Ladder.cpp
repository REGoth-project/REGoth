#include <components/VobClasses.h>
#include "Ladder.h"
#include <logic/PlayerController.h>

using namespace Logic;
using namespace MobCores;

Ladder::Ladder(World::WorldInstance& world, const Handle::EntityHandle& entity)
    : MobCore(world, entity)
{
}

Ladder::~Ladder()
{
}

void Ladder::onBeginStateChange(Handle::EntityHandle npc, int from, int to)
{
    MobCore::onBeginStateChange(npc, from, to);

    VobTypes::NpcVobInformation n = VobTypes::asNpcVob(m_World, npc);
    if (n.isValid())
    {
        // NPC shouldn't be put on ground while using the ladder
        n.playerController->setPhysicsEnabled(false);
    }
}

void Ladder::onEndStateChange(Handle::EntityHandle npc, int from, int to)
{
    MobCore::onEndStateChange(npc, from, to);

    VobTypes::NpcVobInformation n = VobTypes::asNpcVob(m_World, npc);
    if (n.isValid() && to <= 0)
    {
        // If the animation is done, we can put the npc on ground again
        n.playerController->setPhysicsEnabled(true);
    }

    if (to >= 1)
        m_StateNum = 0;  // Put the NPC back to "grabbed the ladder"
}
