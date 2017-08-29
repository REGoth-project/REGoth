//
// Created by kapitaenzufall on 15.08.17.
//

#include "LogManager.h"
#include <engine/World.h>
#include <utils/logger.h>
#include <ui/DialogBox.h>
#include <components/VobClasses.h>
#include <engine/BaseEngine.h>
#include <logic/PlayerController.h>
#include <ui/SubtitleBox.h>
#include <ui/PrintScreenMessages.h>
#include <ui/Hud.h>
#include <logic/visuals/ModelVisual.h>
#include <engine/AsyncAction.h>

using namespace Logic;

LogManager::LogManager(World::WorldInstance& world) :
    m_World(world)
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
