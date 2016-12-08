#pragma once
#include <handle/HandleDef.h>
#include <daedalus/DaedalusGameState.h>
#include <json.hpp>

using json = nlohmann::json;

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
        Daedalus::GameState::ItemHandle addItem(const std::string& symName, unsigned int count = 1);

        /**
         * Removes an item of the given instance from the inventory
         * @param symName Instance to remove
         * @return True, if the item was successfully removed, false if not found
         */
		bool removeItem(const std::string& symName, unsigned int count = 1);
		bool removeItem(size_t symIndex, unsigned int count = 1);
        bool removeItem(Daedalus::GameState::ItemHandle item, unsigned int count = 1);

        /**
         * Removes all items
         */
        void clear();

        /**
         * Returns a random instance of the given item
         * @param symIndex Item to look for
         * @return Handle to an item of the given found in the inventory. Invalid if none could be found.
         */
        Daedalus::GameState::ItemHandle getItem(size_t symIndex);
        Daedalus::GameState::ItemHandle getItem(const std::string& sym);

		/**
		 * @return Count of how many items of the given type are in this inventory
		 */
		int getItemCount(size_t symIndex);

        /**
         * @param symName Name of the symbol to get the instances from
         * @return List of handles of the given instance-name inside this inventory
         */
        const std::list<Daedalus::GameState::ItemHandle>& getItems();

        /**
         * Exports this inventory as JSON
         * @param j JSON-Object to export to
         */
        void exportInventory(json& j);

        /**
         * Imports items from the given json-object.
         * @param j pairs of instance-name and count
         */
        void importInventory(const json& j);
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