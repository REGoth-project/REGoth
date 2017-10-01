//
// Created by markus on 23.05.17.
//

#include "GameSession.h"
#include "GameEngine.h"
#include <fstream>
#include "AsyncAction.h"
#include "ui/Hud.h"
#include "ui/LoadingScreen.h"
#include <components/VobClasses.h>
#include <logic/PlayerController.h>

using namespace Engine;

GameSession::GameSession(GameEngine& engine)
    : m_Engine(engine)
{
    m_CurrentSlotIndex = -1;
}

GameSession::~GameSession()
{
    removeAllWorlds();
}

void GameSession::addInactiveWorld(const std::string& worldName, nlohmann::json&& exportedWorld)
{
    m_InactiveWorlds[worldName] = exportedWorld;
}

bool GameSession::hasInactiveWorld(const std::string& worldName)
{
    return m_InactiveWorlds.find(worldName) != m_InactiveWorlds.end();
}

std::map<std::string, nlohmann::json>& GameSession::getInactiveWorlds()
{
    return m_InactiveWorlds;
}

nlohmann::json GameSession::retrieveInactiveWorld(const std::string& worldName)
{
    if (hasInactiveWorld(worldName))
    {
        nlohmann::json worldJson(std::move(m_InactiveWorlds[worldName]));
        m_InactiveWorlds.erase(worldName);
        return worldJson;
    }
    else
        return nlohmann::json();
}

void GameSession::removeAllWorlds()
{
    while (!m_WorldInstances.empty())
    {
        Handle::WorldHandle w = m_Worlds.front();
        removeWorld(w);
    }
    setMainWorld(Handle::WorldHandle::makeInvalidHandle());
}

void GameSession::setMainWorld(Handle::WorldHandle world)
{
    m_MainWorld = world;
}

std::unique_ptr<World::WorldInstance> GameSession::createWorld(const std::string& _worldFile,
                                                               const json& worldJson,
                                                               const json& scriptEngine,
                                                               const json& dialogManager,
                                                               const json& logManager)
{
    std::string worldFile = _worldFile;

    std::unique_ptr<World::WorldInstance> pWorldInstance = std::make_unique<World::WorldInstance>(m_Engine);
    World::WorldInstance& world = *pWorldInstance;

    if (!worldJson.empty())
    {
        worldFile = worldJson["zenfile"];
    }
    if (!worldFile.empty())
    {
        std::vector<uint8_t> zenData;
        m_Engine.getVDFSIndex().getFileData(worldFile, zenData);

        if (zenData.empty())
        {
            LogWarn() << "Failed to find world file: " << worldFile;
            return nullptr;
        }
    }
    if (!world.init(worldFile, worldJson, scriptEngine, dialogManager, logManager))  // expensive operation
    {
        LogError() << "Failed to init world file: " << worldFile;
        return nullptr;
    }
    return pWorldInstance;
}

Handle::WorldHandle GameSession::registerWorld(std::unique_ptr<World::WorldInstance> pWorldInstance)
{
    if (pWorldInstance == nullptr)
        return Handle::WorldHandle::makeInvalidHandle();

    World::WorldInstance& world = *pWorldInstance;

    m_Engine.onWorldCreated(world.getMyHandle());
    m_WorldInstances.push_back(std::move(pWorldInstance));
    m_Worlds.push_back(world.getMyHandle());

    return world.getMyHandle();
}

void GameSession::removeWorld(Handle::WorldHandle world)
{
    if (m_MainWorld.isValid() && m_MainWorld == world)
        m_MainWorld.invalidate();

    m_Worlds.erase(std::remove(m_Worlds.begin(), m_Worlds.end(), world), m_Worlds.end());

    for (auto it = m_WorldInstances.begin(); it != m_WorldInstances.end(); it++)
    {
        if (it->get() == &world.get())
        {
            m_WorldInstances.erase(it);
            break;
        }
    }
    m_Engine.onWorldRemoved(world);
}

void GameSession::switchToWorld(const std::string& worldFile)
{
    auto switchToWorld_ = [worldFile](GameEngine* engine) {

        json newWorldJson;
        json exportedPlayer;
        json scriptEngine;

        /**
         * prolog
         */
        auto exportData = [&](GameEngine* engine) {
            auto strippedWorldName = Utils::uppered(Utils::stripExtension(worldFile));
            engine->getHud().getLoadingScreen().reset("LOADING_" + strippedWorldName + ".TGA");
            engine->getHud().getLoadingScreen().setHidden(false);

            auto& session = engine->getSession();
            auto oldWorld = engine->getMainWorld();
            auto playerEntity = oldWorld.get().getScriptEngine().getPlayerEntity();
            exportedPlayer = oldWorld.get().exportAndRemoveNPC(playerEntity);
            oldWorld.get().getScriptEngine().exportScriptEngine(scriptEngine);

            session.putWorldToSleep(oldWorld);

            if (session.hasInactiveWorld(worldFile))
            {
                newWorldJson = session.retrieveInactiveWorld(worldFile);
            }
            else
            {
                auto slotIndex = engine->getSession().getCurrentSlot();
                if (slotIndex != -1)
                {
                    // try read from disk
                    std::string worldFromDisk = SavegameManager::readWorld(slotIndex, Utils::stripExtension(worldFile));
                    if (!worldFromDisk.empty())
                        newWorldJson = json::parse(worldFromDisk);  // we found the world on disk
                }
            }
        };
        AsyncAction::executeInThread(exportData, engine, ExecutionPolicy::MainThread).wait();

        /**
         * asynchronous part
         */
        std::unique_ptr<World::WorldInstance> pNewWorld = engine->getSession().createWorld(worldFile, newWorldJson, scriptEngine);

        /**
         * epilog
         */
        auto registerWorld_ = [ w = std::move(pNewWorld), exportedPlayer ](GameEngine * engine) mutable
        {
            Handle::WorldHandle newWorld = engine->getSession().registerWorld(std::move(w));
            engine->getSession().setMainWorld(newWorld);
            auto playerNew = newWorld.get().importVobAndTakeControl(exportedPlayer);

            // TODO find out start position after level change
            std::vector<size_t> startpoints = newWorld.get().findStartPoints();

            if (!startpoints.empty())
            {
                auto playerVob = VobTypes::asNpcVob(newWorld.get(), playerNew);
                std::string startpoint = newWorld.get().getWaynet().waypoints[startpoints.front()].name;
                LogInfo() << "Teleporting player to startpoint '" << startpoint << "'";
                playerVob.playerController->teleportToWaypoint(startpoints.front());
                // FIXME seems like player start-points (zCVobStartpoint:zCVob) are inverted?
                playerVob.playerController->setDirection(-1 * playerVob.playerController->getDirection());
            }
            engine->getHud().getLoadingScreen().setHidden(true);
        };
        AsyncAction::executeInThread(std::move(registerWorld_), engine, ExecutionPolicy::MainThread);
    };
    AsyncAction::executeInThread(switchToWorld_, &m_Engine, ExecutionPolicy::NewThread, true);
}

void GameSession::putWorldToSleep(Handle::WorldHandle worldHandle)
{
    json worldJson;
    worldHandle.get().exportWorld(worldJson);
    addInactiveWorld(worldHandle.get().getZenFile(), std::move(worldJson));
    removeWorld(worldHandle);
}

Handle::WorldHandle GameSession::addWorld(const std::string& worldFile,
                                          const json& worldJson,
                                          const json& scriptEngine,
                                          const json& dialogManager,
                                          const json& logManager)
{
    std::unique_ptr<World::WorldInstance> pWorldInstance = createWorld("", worldJson, scriptEngine, dialogManager, logManager);
    return registerWorld(std::move(pWorldInstance));
}

void GameSession::startNewGame(const std::string& worldFile)
{
    Engine::AsyncAction::JobType<void> addWorld = [worldFile](Engine::GameEngine* engine) {

        auto prolog = [](Engine::GameEngine* engine) {
            engine->getHud().getLoadingScreen().reset();
            engine->getHud().getLoadingScreen().setHidden(false);
            engine->resetSession();
        };
        AsyncAction::executeInThread(prolog, engine, ExecutionPolicy::MainThread).wait();

        std::unique_ptr<World::WorldInstance> uniqueWorld = engine->getSession().createWorld(worldFile);

        auto registerWorld = [w = std::move(uniqueWorld)](Engine::GameEngine * engine) mutable
        {
            Handle::WorldHandle worldHandle = engine->getSession().registerWorld(std::move(w));
            if (worldHandle.isValid())
            {
                engine->getSession().setMainWorld(worldHandle);
                auto& se = worldHandle.get().getScriptEngine();
                auto player = se.createDefaultPlayer(engine->getEngineArgs().playerScriptname);
                worldHandle.get().takeControlOver(player);
            }
            else
            {
                LogError() << "Failed to add given startup world, world handle is invalid!";
            }
            engine->getHud().getLoadingScreen().setHidden(true);
        };
        AsyncAction::executeInThread(std::move(registerWorld), engine, ExecutionPolicy::MainThread);
    };
    bool synchronous = false;
    auto policy = synchronous ? ExecutionPolicy::MainThread : ExecutionPolicy::NewThread;
    // we never want to execute it right away (if it is on MainThread)
    bool forceQueue = true;
    AsyncAction::executeInThread(addWorld, &m_Engine, policy, forceQueue);
}
