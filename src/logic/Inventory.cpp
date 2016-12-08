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

Daedalus::GameState::ItemHandle Inventory::addItem(const std::string& symName, unsigned int count)
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();

    return vm.getGameState().createInventoryItem(vm.getSymbolIndexByName(symName), m_NPC, count);
}

const std::list<Daedalus::GameState::ItemHandle>& Inventory::getItems()
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();

    return vm.getGameState().getInventoryOf(m_NPC);
}

bool Inventory::removeItem(const std::string& symName, unsigned int count)
{
	// Get script-engine
	Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();

	return vm.getGameState().removeInventoryItem(vm.getSymbolIndexByName(symName), m_NPC, count);
}

bool Inventory::removeItem(size_t symIndex, unsigned int count)
{
	// Get script-engine
	Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();

	return vm.getGameState().removeInventoryItem(symIndex, m_NPC, count);
}

bool Inventory::removeItem(Daedalus::GameState::ItemHandle item, unsigned int count)
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();

    if(!item.isValid())
        return false;

    Daedalus::GEngineClasses::C_Item& data = vm.getGameState().getItem(item);

    return vm.getGameState().removeInventoryItem(data.instanceSymbol, m_NPC, count);
}

Daedalus::GameState::ItemHandle Inventory::getItem(size_t symIndex)
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();
    const std::list<Daedalus::GameState::ItemHandle>& items = getItems();

    for(Daedalus::GameState::ItemHandle h : items)
    {
        Daedalus::GEngineClasses::C_Item& data = vm.getGameState().getItem(h);

        if(data.instanceSymbol == symIndex)
            return h;
    }

    return Daedalus::GameState::ItemHandle();
}

Daedalus::GameState::ItemHandle Inventory::getItem(const std::string& sym)
{
    Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();

    return getItem(vm.getSymbolIndexByName(sym));
}

int Inventory::getItemCount(size_t symIndex)
{
	// Get script-engine
	Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();

	Daedalus::GameState::ItemHandle item = getItem(symIndex);

	if(item.isValid())
		return vm.getGameState().getItem(item).count[0];

	return 0;
}

void Inventory::exportInventory(json& j)
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();

    for(auto item : getItems())
    {
        Daedalus::GEngineClasses::C_Item& data = vm.getGameState().getItem(item);
        std::string instanceName = vm.getVM().getDATFile().getSymbolByIndex(data.instanceSymbol).name;

        // Save instance and amount. Rest will be initialized by script on loading
        j[instanceName] = data.count[0];
    }
}

void Inventory::importInventory(const json& j)
{
    // Make sure the item is empty
    //clear();

    for (auto it = j.begin(); it != j.end(); it++)
    {
        addItem(it.key(), it.value());
    }
}

void Inventory::clear()
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_Engine.getWorldInstance(m_World).getScriptEngine();

    auto items = getItems();

    // Remove all items
    for(auto item : items)
    {
        Daedalus::GEngineClasses::C_Item& data = vm.getGameState().getItem(item);

        removeItem(item, (uint32_t)data.count[0]);
    }

    assert(getItems().empty());
}
