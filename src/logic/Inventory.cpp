#include "Inventory.h"
#include "PlayerController.h"
#include "ScriptEngine.h"
#include <components/VobClasses.h>
#include <engine/BaseEngine.h>

using namespace Logic;

Inventory::Inventory(World::WorldInstance& world, Daedalus::GameState::NpcHandle npc)
    : m_World(world)
    , m_NPC(npc)
{
}

Inventory::~Inventory()
{
}

Daedalus::GameState::ItemHandle Inventory::addItem(size_t sym, unsigned int count)
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_World.getScriptEngine();

    return vm.getGameState().createInventoryItem(sym, m_NPC, count);
}

Daedalus::GameState::ItemHandle Inventory::addItem(const std::string& symName, unsigned int count)
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_World.getScriptEngine();

    return addItem(vm.getSymbolIndexByName(symName), count);
}

const std::list<Daedalus::GameState::ItemHandle>& Inventory::getItems()
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_World.getScriptEngine();

    return vm.getGameState().getInventoryOf(m_NPC);
}

bool Inventory::removeItem(const std::string& symName, unsigned int count)
{
    return removeItem(getItem(symName), count);
}

bool Inventory::removeItem(size_t symIndex, unsigned int count)
{
    return removeItem(getItem(symIndex), count);
}

bool Inventory::removeItem(Daedalus::GameState::ItemHandle item, unsigned int count)
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_World.getScriptEngine();

    if (!item.isValid())
        return false;

    // Unequip if this is currently equipped
    if (getItemCount(item) - count == 0)
    {
        Handle::EntityHandle e = VobTypes::getEntityFromScriptInstance(m_World, m_NPC);
        VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, e);

        npc.playerController->unequipItem(item);
    }

    Daedalus::GEngineClasses::C_Item& data = vm.getGameState().getItem(item);

    return vm.getGameState().removeInventoryItem(data.parSymbolIndex, m_NPC, count);
}

Daedalus::GameState::ItemHandle Inventory::getItem(size_t symIndex)
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_World.getScriptEngine();
    const std::list<Daedalus::GameState::ItemHandle>& items = getItems();

    for (Daedalus::GameState::ItemHandle h : items)
    {
        Daedalus::GEngineClasses::C_Item& data = vm.getGameState().getItem(h);

        if (data.parSymbolIndex == symIndex)
            return h;
    }

    return Daedalus::GameState::ItemHandle();
}

Daedalus::GameState::ItemHandle Inventory::getItem(const std::string& sym)
{
    Logic::ScriptEngine& vm = m_World.getScriptEngine();

    return getItem(vm.getSymbolIndexByName(sym));
}

Daedalus::GEngineClasses::C_Item& Inventory::getItem(Daedalus::GameState::ItemHandle item)
{
    Logic::ScriptEngine& vm = m_World.getScriptEngine();

    return vm.getGameState().getItem(item);
}

unsigned int Inventory::getItemCount(size_t symIndex)
{
    Daedalus::GameState::ItemHandle item = getItem(symIndex);

    return getItemCount(item);
}

unsigned int Inventory::getItemCount(Daedalus::GameState::ItemHandle item)
{
    Logic::ScriptEngine& vm = m_World.getScriptEngine();

    if (item.isValid())
        return vm.getGameState().getItem(item).amount;

    return 0;
}

void Inventory::exportInventory(json& j)
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_World.getScriptEngine();

    for (auto item : getItems())
    {
        Daedalus::GEngineClasses::C_Item& data = vm.getGameState().getItem(item);
        std::string instanceName = vm.getVM().getDATFile().getSymbolByIndex(data.parSymbolIndex).name;

        // Save instance and amount. Rest will be initialized by script on loading
        j[instanceName] = data.amount;
    }
}

void Inventory::importInventory(const json& j)
{
    // Make sure the item is empty
    clear();

    for (auto it = j.begin(); it != j.end(); it++)
    {
        addItem(it.key(), it.value());
    }
}

void Inventory::clear()
{
    // Get script-engine
    Logic::ScriptEngine& vm = m_World.getScriptEngine();

    auto items = getItems();

    // Remove all items
    for (auto item : items)
    {
        Daedalus::GEngineClasses::C_Item& data = vm.getGameState().getItem(item);

        removeItem(item, data.amount);
    }

    assert(getItems().empty());
}
