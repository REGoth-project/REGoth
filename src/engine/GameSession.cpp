//
// Created by markus on 23.05.17.
//

#include "GameSession.h"
#include <fstream>
#include <components/VobClasses.h>
#include <logic/PlayerController.h>

using namespace Engine;

GameSession::GameSession(BaseEngine& engine) :
    m_Engine(engine)
{
    m_CurrentSlotIndex = -1;
}

GameSession::~GameSession() {
    removeAllWorlds();
}

void GameSession::addInactiveWorld(const std::string& worldName, nlohmann::json&& exportedWorld)
{
    m_InactiveWorlds[worldName] = exportedWorld;
}

bool GameSession::hasInactiveWorld(const std::string &worldName)
{
    return m_InactiveWorlds.find(worldName) != m_InactiveWorlds.end();
}

std::map<std::string, nlohmann::json>& GameSession::getInactiveWorlds()
{
    return m_InactiveWorlds;
}

nlohmann::json GameSession::retrieveInactiveWorld(const std::string &worldName)
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

GameClock &GameSession::getGameClock()
{
    return m_GameClock;
}

void GameSession::removeAllWorlds()
{
    while(!m_WorldInstances.empty())
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
                                          const json& dialogManager)
{
    std::string worldFile = _worldFile;

    std::unique_ptr<World::WorldInstance> pWorldInstance = std::make_unique<World::WorldInstance>(m_Engine);
    World::WorldInstance& world = *pWorldInstance;

    if (!worldJson.empty())
    {
        worldFile = worldJson["zenfile"];
    }
    if(!worldFile.empty())
    {
        std::vector<uint8_t> zenData;
        m_Engine.getVDFSIndex().getFileData(worldFile, zenData);

        if (zenData.empty())
        {
            LogWarn() << "Failed to find world file: " << worldFile;
            return nullptr;
        }
    }
    if (!world.init(worldFile, worldJson, scriptEngine, dialogManager)) // expensive operation
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

    for(auto it = m_WorldInstances.begin(); it != m_WorldInstances.end(); it++)
    {
        if(it->get() == &world.get())
        {
            m_WorldInstances.erase(it);
            break;
        }
    }
    m_Engine.onWorldRemoved(world);
}

Handle::WorldHandle GameSession::switchToWorld(const std::string &worldFile)
{
    auto oldWorld = getMainWorld();
    auto exportedPlayer = oldWorld.get().exportAndRemoveNPC(
            oldWorld.get().getScriptEngine().getPlayerEntity());
    json scriptEngine;
    oldWorld.get().getScriptEngine().exportScriptEngine(scriptEngine);

    putWorldToSleep(oldWorld);

    json newWorldJson;
    if (hasInactiveWorld(worldFile))
    {
        newWorldJson = retrieveInactiveWorld(worldFile);
    } else
    {
        if (m_CurrentSlotIndex != -1)
        {
            // try read from disk
            std::string worldFromDisk = SavegameManager::readWorld(m_CurrentSlotIndex, Utils::stripExtension(worldFile));
            if (!worldFromDisk.empty())
                newWorldJson = json::parse(worldFromDisk); // we found the world on disk
        }
    }

    // TODO do this asynchronous
    Handle::WorldHandle newWorld;
    {
        std::unique_ptr<World::WorldInstance> pNewWorld = createWorld(worldFile, newWorldJson, scriptEngine);
        newWorld = registerWorld(std::move(pNewWorld));
    }

    setMainWorld(newWorld);
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

    return Handle::WorldHandle();
}

void GameSession::putWorldToSleep(Handle::WorldHandle worldHandle) {
    json worldJson;
    worldHandle.get().exportWorld(worldJson);
    addInactiveWorld(worldHandle.get().getZenFile(), std::move(worldJson));
    removeWorld(worldHandle);
}

Handle::WorldHandle GameSession::addWorld(const std::string& worldFile,
                                          const json& worldJson,
                                          const json& scriptEngine,
                                          const json& dialogManager)
{
    std::unique_ptr<World::WorldInstance> pWorldInstance = createWorld("", worldJson, scriptEngine, dialogManager);
    return registerWorld(std::move(pWorldInstance));
}
