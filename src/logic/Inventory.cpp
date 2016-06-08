#include "Inventory.h"
#include "ScriptEngine.h"
#include <engine/BaseEngine.h>

using namespace Logic;

Inventory::Inventory(Engine::BaseEngine& engine, Handle::WorldHandle world, Daedalus::GameState::NpcHandle npc)
    : m_Engine(engine),
      m_World(world),
      m_NPC(npc)
{

}

Inventory::~Inventory()
{

}

Daedalus::GameState::ItemHandle Inventory::addItem(const std::string& symName)
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();

    return vm.getGameState().addInventoryItem(vm.getSymbolIndexByName(symName), m_NPC);
}

const std::list<Daedalus::GameState::ItemHandle>& Inventory::getItems()
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();

    return vm.getGameState().getInventoryOf(m_NPC);
}

bool Inventory::removeItem(const std::string& symName)
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();

    return vm.getGameState().removeInventoryItem(vm.getSymbolIndexByName(symName), m_NPC);
}
