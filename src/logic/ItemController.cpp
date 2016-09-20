#include "ItemController.h"
#include <engine/World.h>
#include <components/Vob.h>

using namespace Logic;

ItemController::ItemController(World::WorldInstance& world, Handle::EntityHandle entity, Daedalus::GameState::ItemHandle scriptInstance) :
	Controller(world, entity)
{
	m_ScriptState.scriptInstance = scriptInstance;
}

void Logic::ItemController::updateVobFromScript()
{
	Daedalus::GEngineClasses::C_Item& item = m_World.getScriptEngine().getGameState().getItem(m_ScriptState.scriptInstance);
	Vob::VobInformation vob = Vob::asVob(m_World, m_Entity);

	// Update visual
	Vob::setVisual(vob, item.visual);
}
