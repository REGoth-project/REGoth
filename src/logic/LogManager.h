#pragma once

#include <json.hpp>
#include <daedalus/DaedalusDialogManager.h>
#include <daedalus/DaedalusGameState.h>
#include <logic/messages/EventMessage.h>

using json = nlohmann::json;

namespace World
{
    class WorldInstance;
}

namespace Logic
{
    class LogManager
    {
    public:
        LogManager(World::WorldInstance& world);
        ~LogManager();

        /**
         * Get the current player log
         */
        std::map<std::string, Daedalus::GameState::LogTopic>& getPlayerLog();

        /**
         * Export the current log to the given log parameter
         */
        void exportLogManager(json& log);

        /**
         * Import the log from the given log parameter
         */
        void importLogManager(const json& log);

    private:
        /**
         * World this runs in
         */
        World::WorldInstance& m_World;
    };
}
