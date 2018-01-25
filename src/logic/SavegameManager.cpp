#include "SavegameManager.h"
#include <fstream>
#include "engine/GameEngine.h"
#include "ui/Hud.h"
#include "ui/LoadingScreen.h"
#include <json/json.hpp>
#include <utils/Utils.h>
#include <utils/logger.h>
#include <logic/ScriptEngine.h>
#include <logic/DialogManager.h>

using json = nlohmann::json;
using namespace Engine;

/**
 * Gameengine-instance pointer
 */
Engine::GameEngine* gameEngine;

// Enures that all folders to save into the given savegame-slot exist
void ensureSavegameFolders(int idx)
{
    std::string userdata = Utils::getUserDataLocation();

    if (!Utils::mkdir(userdata))
        LogError() << "Failed to create userdata-directory at: " << userdata;

    std::string gameType = "/" + SavegameManager::gameSpecificSubFolderName();

    if (!Utils::mkdir(userdata + gameType))
        LogError() << "Failed to create gametype-directory at: " << userdata + gameType;

    if (!Utils::mkdir(SavegameManager::buildSavegamePath(idx)))
        LogError() << "Failed to create savegame-directory at: " << SavegameManager::buildSavegamePath(idx);
}

std::string SavegameManager::buildSavegamePath(int idx)
{
    std::string userdata = Utils::getUserDataLocation();
    return userdata + "/" + SavegameManager::gameSpecificSubFolderName() + "/savegame_" + std::to_string(idx);
}

std::vector<std::string> SavegameManager::getSavegameWorlds(int idx)
{
    std::vector<std::string> worlds;

    Utils::forEachFile(buildSavegamePath(idx), [&](const std::string& path, const std::string& name, const std::string& ext) {

        // Check if file is empty
        if (Utils::getFileSize(path + "/" + name) == 0)
            return;  // Empty, don't bother

        // Valid worldfile
        worlds.push_back(name);
    });

    return worlds;
}

void SavegameManager::clearSavegame(int idx)
{
    if (!isSavegameAvailable(idx))
        return;  // Don't touch any files if we don't have to...

    Utils::forEachFile(buildSavegamePath(idx), [](const std::string& path, const std::string& name, const std::string& ext) {
        // Make sure this is a REGoth-file
        bool isRegothFile = Utils::endsWith(name, ".json") &&
                            (Utils::startsWith(name, "regoth_") || Utils::startsWith(name, "world_") || Utils::startsWith(name, "player") || Utils::startsWith(name, "dialogmanager") || Utils::startsWith(name, "scriptengine"));

        if (!isRegothFile)
            return;  // Better not touch that one

        // Empty the file
        FILE* f = fopen(path.c_str(), "w");
        if (!f)
        {
            LogWarn() << "Failed to clear file: " << path;
            return;
        }

        fclose(f);

    },
                       false);  // For the love of god, dont recurse in case something really goes wrong!
}

bool SavegameManager::isSavegameAvailable(int idx)
{
    return Utils::getFileSize(buildSavegamePath(idx) + "/regoth_save.json") > 0;
}

bool SavegameManager::writeSavegameInfo(int idx, const SavegameInfo& info)
{
    std::string infoFile = buildSavegamePath(idx) + "/regoth_save.json";

    ensureSavegameFolders(idx);

    json j;
    j["version"] = info.LATEST_KNOWN_VERSION;
    j["name"] = info.name;
    j["world"] = info.world;
    j["timePlayed"] = info.timePlayed;

    LogInfo() << "Writing savegame-info: " << infoFile;

    // Save
    std::ofstream f(infoFile);

    if (!f.is_open())
    {
        LogWarn() << "Failed to save data! Could not open file: " << buildSavegamePath(idx) + "/regoth_save.json";
        return false;
    }

    f << j.dump(4);
    f.close();

    return true;
}

Engine::SavegameManager::SavegameInfo SavegameManager::readSavegameInfo(int idx)
{
    std::string info = buildSavegamePath(idx) + "/regoth_save.json";

    if (!Utils::getFileSize(info))
        return SavegameInfo();

    LogInfo() << "Reading savegame-info: " << info;

    std::string infoContents = Utils::readFileContents(info);
    json j = json::parse(infoContents);

    unsigned int version = 0;
    // check can be removed if backwards compability with save games without version number is not needed anymore
    if (j.find("version") != j.end())
    {
        version = j["version"];
    }
    SavegameInfo o;
    o.version = version;
    o.name = j["name"];
    o.world = j["world"];
    o.timePlayed = j["timePlayed"];

    return o;
}

bool SavegameManager::writePlayer(int idx, const std::string& playerName, const nlohmann::json& player)
{
    return writeFileInSlot(idx, playerName + ".json", Utils::iso_8859_1_to_utf8(player.dump()));
}

std::string SavegameManager::readPlayer(int idx, const std::string& playerName)
{
    return SavegameManager::readFileInSlot(idx, playerName + ".json");
}

bool SavegameManager::writeWorld(int idx, const std::string& worldName, const nlohmann::json& world)
{
    return writeFileInSlot(idx, "world_" + worldName + ".json", Utils::iso_8859_1_to_utf8(world.dump(4)));
}

std::string SavegameManager::readWorld(int idx, const std::string& worldName)
{
    return SavegameManager::readFileInSlot(idx, "world_" + worldName + ".json");
}

std::string SavegameManager::buildWorldPath(int idx, const std::string& worldName)
{
    return buildSavegamePath(idx) + "/world_" + worldName + ".json";
}

void Engine::SavegameManager::init(Engine::GameEngine& engine)
{
    gameEngine = &engine;
}

std::vector<std::shared_ptr<const std::string>> SavegameManager::gatherAvailableSavegames()
{
    int numSlots = maxSlots();

    std::vector<std::shared_ptr<const std::string>> names(numSlots, nullptr);

    // Try every slot
    for (int i = 0; i < numSlots; ++i)
    {
        if (isSavegameAvailable(i))
        {
            SavegameInfo info = readSavegameInfo(i);
            names[i] = std::make_shared<const std::string>(info.name);
        }
    }
    // for log purpose only
    {
        std::vector<std::string> names2;
        for (auto& namePtr : names)
        {
            if (namePtr)
                names2.push_back(*namePtr);
            else
                names2.push_back("");
        }
        LogInfo() << "Available savegames: " << names2;
    }

    return names;
}

std::string Engine::SavegameManager::loadSaveGameSlot(int index)
{
    ExcludeFrameTime exclude(*gameEngine);
    // Lock to number of savegames
    assert(index >= 0 && index < maxSlots());

    if (!isSavegameAvailable(index))
    {
        return "Savegame at slot " + std::to_string(index) + " not available!";
    }

    // Read general information about the saved game. Most importantly the world the player saved in
    SavegameInfo info = readSavegameInfo(index);

    std::string worldFileData = SavegameManager::readWorld(index, info.world);
    // Sanity check, if we really got a safe for this world. Otherwise we would end up in the fresh version
    // if it was missing. Also, IF the player saved there, there should be a save for this.
    if (worldFileData.empty())
    {
        return "Target world-file invalid: " + buildWorldPath(index, info.world);
    }
    auto timePlayed = info.timePlayed;
    auto loadSave = [worldFileData, index, timePlayed](BaseEngine* engine) {
        auto resetSession = [](BaseEngine* engine) {
            engine->resetSession();
            engine->getHud().getLoadingScreen().reset();
            engine->getHud().getLoadingScreen().setHidden(false);
        };
        engine->executeInMainThread<void>(resetSession).wait();

        json worldJson = json::parse(worldFileData);
        // TODO: catch json exception when emtpy file is parsed or parser crashes
        json scriptEngine = json::parse(SavegameManager::readFileInSlot(index, "scriptengine.json"));
        json dialogManager = json::parse(SavegameManager::readFileInSlot(index, "dialogmanager.json"));
        json logManager = json::parse(SavegameManager::readFileInSlot(index, "logmanager.json"));
        engine->getSession().setCurrentSlot(index);
        engine->getGameClock().setTotalSeconds(timePlayed);
        using UniqueWorld = std::unique_ptr<World::WorldInstance>;
        std::shared_ptr<UniqueWorld> pWorld;
        pWorld = std::make_shared<UniqueWorld>(engine->getSession().createWorld("", worldJson, scriptEngine, dialogManager, logManager));

        auto registerWorld = [index, pWorld](BaseEngine * engine)
        {
            Handle::WorldHandle worldHandle = engine->getSession().registerWorld(std::move(*pWorld));
            if (worldHandle.isValid())
            {
                engine->getSession().setMainWorld(worldHandle);
                json playerJson = json::parse(readPlayer(index, "player"));
                engine->getMainWorld().get().importVobAndTakeControl(playerJson);
            }
            engine->getHud().getLoadingScreen().setHidden(true);
        };
        engine->executeInMainThread<void>(std::move(registerWorld));
    };
    gameEngine->executeInThread<void>(loadSave, ExecutionPolicy::NewThread);
    return "";
}

int Engine::SavegameManager::maxSlots()
{
    switch (gameEngine->getBasicGameType())
    {
        case Daedalus::GameType::GT_Gothic1:
            return G1_MAX_SLOTS;
        case Daedalus::GameType::GT_Gothic2:
            return G2_MAX_SLOTS;
        default:
            return G2_MAX_SLOTS;
    }
}

void Engine::SavegameManager::saveToSlot(int index, std::string savegameName)
{
    if (!gameEngine->getMainWorld().isValid() || gameEngine->getMainWorld().get().getDialogManager().isDialogActive())
        return; // only save while not in Dialog

    ExcludeFrameTime exclude(*gameEngine);
    assert(index >= 0 && index < SavegameManager::maxSlots());

    if (savegameName.empty())
        savegameName = std::string("Slot") + std::to_string(index);

    // TODO: Should be writing to a temp-directory first, before messing with the save-files already existing
    // Clean data from old savegame, so we don't load into worlds we haven't been to yet
    Engine::SavegameManager::clearSavegame(index);

    World::WorldInstance& mainWorld = gameEngine->getMainWorld().get();
    // Write information about the current game-state
    Engine::SavegameManager::SavegameInfo info;
    info.version = Engine::SavegameManager::SavegameInfo::LATEST_KNOWN_VERSION;
    info.name = savegameName;
    info.world = Utils::stripExtension(mainWorld.getZenFile());
    info.timePlayed = gameEngine->getGameClock().getTotalSeconds();

    Engine::SavegameManager::writeSavegameInfo(index, info);

    // export left worlds we visited in this session
    for (auto& pair : gameEngine->getSession().getInactiveWorlds())
    {
        std::string worldName = Utils::stripExtension(pair.first);
        nlohmann::json& worldJson = pair.second;
        SavegameManager::writeWorld(index, worldName, worldJson);
    }
    // no need to keep them in memory anymore and they would be unnecessarily saved each time
    gameEngine->getSession().getInactiveWorlds().clear();

    // export player
    json playerJson = mainWorld.exportNPC(mainWorld.getScriptEngine().getPlayerEntity());
    Engine::SavegameManager::writePlayer(index, "player", playerJson);

    // export mainWorld, but skip the player
    json mainWorldjson;
    mainWorld.exportWorld(mainWorldjson, {mainWorld.getScriptEngine().getPlayerEntity()});
    Engine::SavegameManager::writeWorld(index, info.world, mainWorldjson);

    // export dialog info
    json dialogManager;
    mainWorld.getDialogManager().exportDialogManager(dialogManager);
    Engine::SavegameManager::writeFileInSlot(index, "dialogmanager.json", Utils::iso_8859_1_to_utf8(dialogManager.dump(4)));

    // export log info
    json logManager;
    gameEngine->getSession().getLogManager().exportLogManager(logManager);
    Engine::SavegameManager::writeFileInSlot(index, "logmanager.json", Utils::iso_8859_1_to_utf8(logManager.dump(4)));

    // export script engine
    json scriptEngine;
    mainWorld.getScriptEngine().exportScriptEngine(scriptEngine);
    Engine::SavegameManager::writeFileInSlot(index, "scriptengine.json", Utils::iso_8859_1_to_utf8(scriptEngine.dump(4)));
    gameEngine->getSession().setCurrentSlot(index);
}

std::string Engine::SavegameManager::gameSpecificSubFolderName()
{
    if (gameEngine->getBasicGameType() == Daedalus::GameType::GT_Gothic1)
        return "Gothic";
    else
        return "Gothic 2";
}

std::string Engine::SavegameManager::readFileInSlot(int idx, const std::string& relativePath)
{
    std::string file = buildSavegamePath(idx) + "/" + relativePath;

    if (!Utils::getFileSize(file))
        return "";  // Not found or empty

    LogInfo() << "Reading save-file: " << file;
    return Utils::readFileContents(file);
}

bool Engine::SavegameManager::writeFileInSlot(int idx, const std::string& relativePath, const std::string& data)
{
    std::string file = buildSavegamePath(idx) + "/" + relativePath;
    ensureSavegameFolders(idx);

    LogInfo() << "Writing save-file: " << file;

    std::ofstream f(file);
    if (!f.is_open())
    {
        LogWarn() << "Failed to save data! Could not open file: " + file;
        return false;
    }

    f << data;

    return true;
}
