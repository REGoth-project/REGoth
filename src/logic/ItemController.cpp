#include "ItemController.h"
#include <engine/World.h>
#include <components/Vob.h>
#include <components/VobClasses.h>
#include "PlayerController.h"

using namespace Logic;

ItemController::ItemController(World::WorldInstance& world, Handle::EntityHandle entity, size_t scriptInstance) :
	Controller(world, entity)
{
	m_ScriptState.scriptInstance = scriptInstance;
	m_World.getScriptEngine().registerItem(m_Entity);

	Vob::VobInformation vob = Vob::asVob(m_World, m_Entity);
    Vob::setCollisionEnabled(vob, false); // FIXME: Should be true, but right now there is no way to remove physics objects from the world without crashing
}

void ItemController::pickUp(Handle::EntityHandle npc)
{
	VobTypes::NpcVobInformation npcvob = VobTypes::asNpcVob(m_World, npc);

	if(npcvob.isValid())
	{
		npcvob.playerController->getInventory().addItem(m_ScriptState.scriptInstance);
	}

	remove();
}

void ItemController::remove()
{
	// Unregister item from world first
	m_World.getScriptEngine().unregisterItem(m_Entity);

	// Remove this vob from the world
	m_World.removeEntity(m_Entity);
}


void ItemController::importObject(const json& j)
{
    Controller::importObject(j);
}

void ItemController::exportPart(json& j)
{
	Controller::exportPart(j);

	j["type"] = "ItemController";
    j["instanceSymbol"] = m_ScriptState.scriptInstance;
}

