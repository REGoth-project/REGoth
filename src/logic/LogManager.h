#pragma once

#include <daedalus/DaedalusDialogManager.h>
#include <logic/messages/EventMessage.h>
#include <daedalus/DaedalusGameState.h>

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

	private:

        /**
         * World this runs in
         */
        World::WorldInstance& m_World;
    };
}
