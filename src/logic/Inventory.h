#pragma once
#include <handle/HandleDef.h>
#include <daedalus/DaedalusGameState.h>

namespace Engine
{
    class BaseEngine;
}

namespace Logic
{
    class Inventory
    {
    public:
        Inventory(Engine::BaseEngine& engine, Handle::WorldHandle world, Daedalus::GameState::NpcHandle npc);
        virtual ~Inventory();

        /**
         * Creates and adds an item of the underlaying script instance
         * @param symName Name of the script-symbol
         * @return Handle to the created item
         */
        Daedalus::GameState::ItemHandle addItem(const std::string& symName);

        /**
         * Removes an item of the given instance from the inventory
         * @param symName Instance to remove
         * @return True, if the item was successfully removed, false if not found
         */
        bool removeItem(const std::string& symName);
        bool removeItem(Daedalus::GameState::ItemHandle item);

        /**
         * Returns a random instance of the given item
         * @param symIndex Item to look for
         * @return Handle to an item of the given found in the inventory. Invalid if none could be found.
         */
        Daedalus::GameState::ItemHandle getItem(size_t symIndex);

        /**
         * @param symName Name of the symbol to get the instances from
         * @return List of handles of the given instance-name inside this inventory
         */
        const std::list<Daedalus::GameState::ItemHandle>& getItems();
    protected:

        /**
         * NPC this inventory belongs to
         */
        Daedalus::GameState::NpcHandle m_NPC;

        /**
         * Internal
         */
        Engine::BaseEngine& m_Engine;
        Handle::WorldHandle m_World;
    };
}