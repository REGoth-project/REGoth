//
// Created by markus on 23.05.17.
//
#pragma once
#include <map>
#include <memory>
#include <list>
#include <json/json.hpp>
#include <handle/HandleDef.h>
#include "World.h"
#include "GameClock.h"
#include "BaseEngine.h"

namespace Engine
{
    class GameSession {
    public:
        GameSession(BaseEngine& engine);

        /**
         * Cleans current session
         */
        ~GameSession();

        /**
         * Store already visited worlds of the current session
         * @param worldName zen filename including extension
         * @param exportedWorld world as json object without players in it
         */
        void addInactiveWorld(const std::string& worldName, nlohmann::json&& exportedWorld);

        /**
         * Get the remembered World and removes it from the map if it exists, else returns empty json
         * @param worldName zen filename including extension
         */
        nlohmann::json retrieveInactiveWorld(const std::string &worldName);

        /**
         * @return refernce to all inactive Worlds of the current session
         */
        std::map<std::string, nlohmann::json>& getInactiveWorlds();

        /**
         * @param worldName zen filename including extension
         * @return whether a world is currently unloaded to memory as json object
         */
        bool hasInactiveWorld(const std::string& worldName);

        /**
         * @return Gameclock
         */
        GameClock& getGameClock();

        void setCurrentSlot(int index) { m_CurrentSlotIndex = index; }

        std::map<size_t, std::set<size_t>>& getKnownInfoMap() { return m_KnownInfos; };

        /**
         * @return data-access to the main world of this session
         */
        Handle::WorldHandle getMainWorld()
        {
            return m_MainWorld;
        }

        /**
         * @return const reference to the list of unqiue pointers to loaded worlds
         */
        const std::list<std::unique_ptr<World::WorldInstance>>& getWorldInstances() { return m_WorldInstances; }

        /**
         * Sets the currently active world.
         * @param world
         */
        void setMainWorld(Handle::WorldHandle world);

        /**
         * @brief Adds a world. If worldJson is non empty, argument worldFile will be ignored
         * @param worldfile Path to look for the worldfile. Can be inside a VDF-Archive
         *		  or on disk (TODO)
         */
        Handle::WorldHandle addWorld(const std::string& worldFile,
                                     const json& worldJson = json(),
                                     const json& scriptEngine = json(),
                                     const json& dialogManger = json());

        /**
         * Switch to world with the given name.
         * The game evaluates the following conditions and chooses the first valid one
         * - world found in inactive worlds (already visited in this session, json object in memory)
         * - world found in current save-game slot on disk
         * - else: First visit. No vobs get imported. <World>_startup script-fu will be executed
         * @param worldFile including .zen extension
         * @return handle to the loaded world
         */
        Handle::WorldHandle switchToWorld(const std::string &worldFile);

        /**
         * @brief moves world from worldInstance list to inactive json map.
         * @param worldHandle
         */
        void putWorldToSleep(Handle::WorldHandle worldHandle);

        /**
         * Removes a world and everything inside
         * @param world World to remove
         */
        void removeWorld(Handle::WorldHandle world);

        /**
         * Removes all worlds and everything inside
         */
        void removeAllWorlds();

    private:
        std::map<std::string, nlohmann::json> m_InactiveWorlds;

        /**
         * last savegame slot used in this session (save or load). Is -1 if "new game" was started
         */
        int m_CurrentSlotIndex;

        /**
         * Gameclock
         */
        GameClock m_GameClock;

        /**
         * known infoInstances by npcInstances
         */
        std::map<size_t, std::set<size_t>> m_KnownInfos;

        /**
         * Currently active world instances
         */
        std::list<std::unique_ptr<World::WorldInstance>> m_WorldInstances;

        /**
         * Main world of this engine-instance
         */
        Handle::WorldHandle m_MainWorld;

        /**
         * Registered worlds
         */
        std::vector<Handle::WorldHandle> m_Worlds;

        /**
         * reference to base engine
         */
        BaseEngine& m_Engine;
    };

}