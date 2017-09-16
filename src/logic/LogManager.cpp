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

void LogManager::importLogManager(const json& log)
{
    std::map<std::string, Daedalus::GameState::LogTopic>& playerLog = getPlayerLog();

    for (auto it1 = log.begin(); it1 != log.end(); it1++)
    {
        for (auto it2 = log.at(it1.key()).begin(); it2 != log.at(it1.key()).end(); it2++)
        {
            if (it1.key() == "mission")
            {
                for (auto it3 = it2.value().begin(); it3 != it2.value().end(); it3++)
                {
                    json missionTopic = it3.value();
                    std::string name = Utils::utf8_to_iso8859_1(missionTopic.at("name").get<std::string>().c_str());

                    if (it2.key() == "running")
                    {
                        playerLog[name].status = Daedalus::GameState::LogTopic::ELogStatus::LS_Running;
                    }
                    else if (it2.key() == "success")
                    {
                        playerLog[name].status = Daedalus::GameState::LogTopic::ELogStatus::LS_Success;
                    }
                    else if (it2.key() == "failed")
                    {
                        playerLog[name].status = Daedalus::GameState::LogTopic::ELogStatus::LS_Failed;
                    }
                    else if (it2.key() == "obsolete")
                    {
                        playerLog[name].status = Daedalus::GameState::LogTopic::ELogStatus::LS_Obsolete;
                    }

                    playerLog[name].section = Daedalus::GameState::LogTopic::ESection::LT_Mission;

                    for (auto it4 = missionTopic.at("entries").begin(); it4 != missionTopic.at("entries").end(); it4++)
                    {
                        playerLog[name].entries.push_back(Utils::utf8_to_iso8859_1(it4.value().get<std::string>().c_str()));
                    }
                }
            }
            else if (it1.key() == "note")
            {
                json noteTopic = it2.value();
                std::string name = Utils::utf8_to_iso8859_1(noteTopic.at("name").get<std::string>().c_str());

                playerLog[name].section = Daedalus::GameState::LogTopic::ESection::LT_Note;

                for (auto it5 = noteTopic.at("entries").begin(); it5 != noteTopic.at("entries").end(); it5++)
                {
                    playerLog[name].entries.push_back(Utils::utf8_to_iso8859_1(it5.value().get<std::string>().c_str()));
                }
            }
        }
    }
}
