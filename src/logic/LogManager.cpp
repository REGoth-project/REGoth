//
// Created by kapitaenzufall on 15.08.17.
//

#include "LogManager.h"
#include <engine/BaseEngine.h>
#include <utils/logger.h>

using namespace Logic;
using LogTopic = Daedalus::GameState::LogTopic;

const std::map<std::string, LogTopic>& Logic::LogManager::getPlayerLog()
{
    return m_PlayerLog;
}

void LogManager::createTopic(const std::string& topicName, LogTopic::ESection section)
{
    m_PlayerLog[topicName].section = section;
}

void LogManager::setTopicStatus(const std::string& topicName, LogTopic::ELogStatus status)
{
    m_PlayerLog[topicName].status = status;
}

void LogManager::addEntry(const std::string& topicName, std::string entry)
{
    m_PlayerLog[topicName].entries.push_back(std::move(entry));
}

void LogManager::exportLogManager(json& log)
{
    log["mission"]["running"] = json::array();
    log["mission"]["success"] = json::array();
    log["mission"]["failed"] = json::array();
    log["mission"]["obsolete"] = json::array();
    log["note"] = json::array();

    for (const auto& topic : m_PlayerLog)
    {
        json jsonTopic;
        jsonTopic["name"] = topic.first;
        jsonTopic["entries"] = topic.second.entries;

        if (topic.second.section == LogTopic::ESection::LT_Mission)
        {
            switch (topic.second.status)
            {
                case LogTopic::ELogStatus::LS_Running:
                    log["mission"]["running"].push_back(jsonTopic);
                    break;
                case LogTopic::ELogStatus::LS_Success:
                    log["mission"]["success"].push_back(jsonTopic);
                    break;
                case LogTopic::ELogStatus::LS_Failed:
                    log["mission"]["failed"].push_back(jsonTopic);
                    break;
                case LogTopic::ELogStatus::LS_Obsolete:
                    log["mission"]["obsolete"].push_back(jsonTopic);
                    break;
            }
        }
        else if (topic.second.section == LogTopic::ESection::LT_Note)
        {
            log["note"].push_back(jsonTopic);
        }
    }
}

void LogManager::importTopic(const json& topic, LogTopic::ESection section, LogTopic::ELogStatus status)
{
    std::string name = Utils::utf8_to_iso8859_1(topic.at("name").get<std::string>().c_str());
    createTopic(name, section);
    setTopicStatus(name, status);
    for (auto entry : topic.at("entries"))
        addEntry(name, Utils::utf8_to_iso8859_1(entry.get<std::string>().c_str()));
}

void LogManager::importLogManager(const json& log)
{
    using ELogStatus = LogTopic::ELogStatus;
    const std::map<std::string, ELogStatus> logStatus {
        std::make_pair("running", ELogStatus::LS_Running),
        std::make_pair("success", ELogStatus::LS_Success),
        std::make_pair("failed", ELogStatus::LS_Failed),
        std::make_pair("obsolete", ELogStatus::LS_Obsolete),
    };

    for (auto missionStatus : json::iterator_wrapper(log.at("mission")))
    {
        auto status = logStatus.at(missionStatus.key());
        for (auto missionTopic : missionStatus.value())
            importTopic(missionTopic, LogTopic::ESection::LT_Mission, status);
    }

    for (auto noteTopic : log.at("note"))
        importTopic(noteTopic, LogTopic::ESection::LT_Note, ELogStatus::LS_Running);
}
