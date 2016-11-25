#include "ItemController.h"
#include <engine/World.h>
#include <components/Vob.h>
#include <components/VobClasses.h>

using namespace Logic;

ItemController::ItemController(World::WorldInstance& world, Handle::EntityHandle entity, Daedalus::GameState::ItemHandle scriptInstance) :
	Controller(world, entity)
{
	m_ScriptState.scriptInstance = scriptInstance;
	m_World.getScriptEngine().registerItem(m_Entity);
}

void Logic::ItemController::updateVobFromScript()
{
	Daedalus::GEngineClasses::C_Item& item = m_World.getScriptEngine().getGameState().getItem(m_ScriptState.scriptInstance);
	Vob::VobInformation vob = Vob::asVob(m_World, m_Entity);

	// Update visual
	Vob::setVisual(vob, item.visual);
}

void ItemController::pickUp(Handle::EntityHandle npc)
{
	VobTypes::NpcVobInformation npcvob = VobTypes::asNpcVob(m_World, npc);

	if(npcvob.isValid())
	{
		// Unregister item from world first
		m_World.getScriptEngine().unregisterItem(m_Entity);

		// Add our script-instance to the npcs inventory
		m_World.getScriptEngine().getGameState().addItemToInventory(m_ScriptState.scriptInstance, VobTypes::getScriptHandle(npcvob));

		// Remove this vob from the world
		m_World.removeEntity(m_Entity);
	}
}
