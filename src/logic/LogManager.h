#pragma once

#include <json.hpp>
#include <daedalus/DaedalusGameState.h>

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
        /**
         * Get the current player log
         */
        const std::map<std::string, Daedalus::GameState::LogTopic>& getPlayerLog();

        /**
         * Creates a new Topic
         */
        void createTopic(const std::string& topicName, Daedalus::GameState::LogTopic::ESection section);

        /**
         * Creates a new Topic
         */
        void setTopicStatus(const std::string& topicName, Daedalus::GameState::LogTopic::ELogStatus status);

        /**
         * Adds an entry to a topic
         */
        void addEntry(const std::string& topicName, std::string entry);

        /**
         * Export the current log to the given log parameter
         */
        void exportLogManager(json& log);

        /**
         * Import the log from the given log parameter
         */
        void importLogManager(const json& log);

        /**
         * imports a single Mission-topic or Note-topic
         */
        void importTopic(const json& topic, Daedalus::GameState::LogTopic::ESection section, Daedalus::GameState::LogTopic::ELogStatus status);

    private:

        /**
         * @brief The players log. Map of Entries by Topics
         */
        std::map<std::string, Daedalus::GameState::LogTopic> m_PlayerLog;
    };
}
