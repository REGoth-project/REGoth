//
// Created by kapitaenzufall on 15.08.17.
//

#include "LogManager.h"
#include <components/VobClasses.h>
#include <engine/AsyncAction.h>
#include <engine/BaseEngine.h>
#include <engine/World.h>
#include <logic/PlayerController.h>
#include <logic/visuals/ModelVisual.h>
#include <ui/DialogBox.h>
#include <ui/Hud.h>
#include <ui/PrintScreenMessages.h>
#include <ui/SubtitleBox.h>
#include <utils/logger.h>

using namespace Logic;

LogManager::LogManager(World::WorldInstance& world)
    : m_World(world)
{
}

LogManager::~LogManager()
{
}

std::map<std::string, Daedalus::GameState::LogTopic>& Logic::LogManager::getPlayerLog()
{
    Logic::ScriptEngine& vm = m_World.getScriptEngine();
    return vm.getGameState().getPlayerLog();
}

void LogManager::exportLogManager(json& log)
{
    std::map<std::string, Daedalus::GameState::LogTopic>& playerLog = getPlayerLog();

    log["mission"]["running"] = json::array();
    log["mission"]["success"] = json::array();
    log["mission"]["failed"] = json::array();
    log["mission"]["obsolete"] = json::array();
    log["note"] = json::array();

    for (const auto& topic : playerLog)
    {
        json jsonTopic;
        jsonTopic["name"] = topic.first;
        jsonTopic["entries"] = topic.second.entries;

        if (topic.second.section == Daedalus::GameState::LogTopic::ESection::LT_Mission)
        {
            switch (topic.second.status)
            {
                case Daedalus::GameState::LogTopic::ELogStatus::LS_Running:
                    log["mission"]["running"].push_back(jsonTopic);
                    break;
                case Daedalus::GameState::LogTopic::ELogStatus::LS_Success:
                    log["mission"]["success"].push_back(jsonTopic);
                    break;
                case Daedalus::GameState::LogTopic::ELogStatus::LS_Failed:
                    log["mission"]["failed"].push_back(jsonTopic);
                    break;
                case Daedalus::GameState::LogTopic::ELogStatus::LS_Obsolete:
                    log["mission"]["obsolete"].push_back(jsonTopic);
                    break;
            }
        }
        else if (topic.second.section == Daedalus::GameState::LogTopic::ESection::LT_Note)
        {
            log["note"].push_back(jsonTopic);
        }
    }
}

void LogManager::importTopic(const json& topic, Daedalus::GameState::LogTopic::ESection section, Daedalus::GameState::LogTopic::ELogStatus status)
{
    std::map<std::string, Daedalus::GameState::LogTopic>& playerLog = getPlayerLog();
    std::string name = Utils::utf8_to_iso8859_1(topic.at("name").get<std::string>().c_str());
    playerLog[name].section = section;
    playerLog[name].status = status;
    for (auto entry : topic.at("entries"))
    {
        playerLog[name].entries.push_back(Utils::utf8_to_iso8859_1(entry.get<std::string>().c_str()));
    }
}

void LogManager::importLogManager(const json& log)
{
    std::map<std::string, Daedalus::GameState::LogTopic>& playerLog = getPlayerLog();
    using ELogStatus = Daedalus::GameState::LogTopic::ELogStatus;
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
            importTopic(missionTopic, Daedalus::GameState::LogTopic::ESection::LT_Mission, status);
    }

    for (auto noteTopic : log.at("note"))
        importTopic(noteTopic, Daedalus::GameState::LogTopic::ESection::LT_Note, ELogStatus::LS_Running);
}
