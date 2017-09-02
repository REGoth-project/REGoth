#include "BaseEngine.h"
#include <fstream>
#include "AsyncAction.h"
#include "World.h"
#include "audio/AudioEngine.h"
#include <bx/commandline.h>
#include <components/EntityActions.h>
#include <components/Vob.h>
#include <components/VobClasses.h>
#include <logic/PlayerController.h>
#include <render/WorldRender.h>
#include <ui/Hud.h>
#include <ui/LoadingScreen.h>
#include <ui/zFont.h>
#include <utils/bgfx_lib.h>
#include <utils/cli.h>
#include <utils/logger.h>
#include <vdfs/fileIndex.h>
#include <zenload/zCMesh.h>
#include <zenload/zCModelPrototype.h>
#include <zenload/zenParser.h>

using namespace Engine;

namespace Flags
{
    Cli::Flag gameDirectory("g", "game-dir", 1, "Root-folder of your Gothic installation", {"."}, "Data");
    Cli::Flag g1Directory("", "g1-path", 1, "Game data to use when the -g1 flag is specified on the commandline", {"."}, "Data");
    Cli::Flag g2Directory("", "g2-path", 1, "Game data to use when the -g2 flag is specified on the commandline", {"."}, "Data");
    Cli::Flag startG1("g1", "start-g1", 0, "Uses the path stored in the 'g1-path' config setting as game data path");
    Cli::Flag startG2("g2", "start-g2", 0, "Uses the path stored in the 'g2-path' config setting as game data path");

    Cli::Flag modFile("m", "mod-file", 1, "Additional .mod-file to load", {""}, "Data");
    Cli::Flag world("w", "world", 1, ".ZEN-file to load out of one of the vdf-archives", {""}, "Data");
    Cli::Flag emptyWorld("", "empty-world", 0, "Will load no .ZEN-file at all.");
    Cli::Flag playerScriptname("p", "player", 1, "When starting a new game the player will play as the given NPC", {"PC_HERO"});
    Cli::Flag startNewGame("", "newgame", 0, "Starts a new game directly instead of showing the menu at startup");
    Cli::Flag sndDevice("snd", "sound-device", 1, "OpenAL sound device", {""}, "Sound");
}

BaseEngine::BaseEngine()
    : m_MainThreadID(std::this_thread::get_id())
    , m_RootUIView(*this)
    , m_Console(*this)
    , m_EngineTextureAlloc(*this)
{
    m_pHUD = nullptr;
    m_pFontCache = nullptr;

    m_BasicGameType = Daedalus::GameType::GT_Gothic2;
    m_Paused = false;
    m_ExcludedFrameTime = 0;
    // allocate and init default session
    resetSession();
}

BaseEngine::~BaseEngine()
{
    getRootUIView().removeChild(m_pHUD);
    delete m_AudioEngine;
    delete m_pHUD;
    delete m_pFontCache;
}

void BaseEngine::initEngine(int argc, char** argv)
{
    m_Args.cmdline = bx::CommandLine(argc, (const char**)argv);
    const char* value = nullptr;

    LogInfo() << "Initializing...";

    m_Args.gameBaseDirectory = ".";
    m_FileIndex.Init(argv[0]);
    //m_Args.startupZEN = "addonworld.zen";

    if (Flags::startG1.isSet())
        m_Args.gameBaseDirectory = Flags::g1Directory.getParam(0);
    else if (Flags::startG2.isSet())
        m_Args.gameBaseDirectory = Flags::g2Directory.getParam(0);
    else if (Flags::gameDirectory.isSet())
        m_Args.gameBaseDirectory = Flags::gameDirectory.getParam(0);
    else
        LogInfo() << "No game-root specified! Using the current working-directory as game root. Use the '-g' flag to specify this!";

    if (Flags::modFile.isSet())
        m_Args.modfile = Flags::modFile.getParam(0);

    if (Flags::world.isSet())
        m_Args.startupZEN = Flags::world.getParam(0);

    loadArchives();

    if (m_Args.startupZEN.empty() || !m_FileIndex.hasFile(m_Args.startupZEN))
    {
        // Try Gothic 1
        if (m_FileIndex.hasFile("world.zen"))
            m_Args.startupZEN = "world.zen";
        else if (m_FileIndex.hasFile("newworld.zen"))
            m_Args.startupZEN = "newworld.zen";
        else
            LogWarn() << "Unknown game files, could not find world.zen or newworld.zen!";
    }

    if (Flags::emptyWorld.isSet())
        m_Args.startupZEN = "";

    if (Flags::playerScriptname.isSet())
        m_Args.playerScriptname = Flags::playerScriptname.getParam(0);

    m_Args.startNewGame = Flags::startNewGame.isSet();

    std::string snd_device;
    if (Flags::sndDevice.isSet())
        snd_device = Flags::sndDevice.getParam(0);

    m_AudioEngine = new Audio::AudioEngine(snd_device);

    // Init HUD
    m_pFontCache = new UI::zFontCache(*this);
    m_pHUD = new UI::Hud(*this);
    getRootUIView().addChild(m_pHUD);
}

void BaseEngine::frameUpdate(double dt, uint16_t width, uint16_t height)
{
    onFrameUpdate(dt * getGameClock().getGameEngineSpeedFactor(), width, height);
}

void BaseEngine::loadArchives()
{
    //m_FileIndex.loadVDF("vdf/Worlds_Addon.vdf");
    //m_FileIndex.loadVDF("vdf/Textures.vdf");
    //m_FileIndex.loadVDF("vdf/Textures_Addon.vdf");
    //m_FileIndex.loadVDF("vdf/Meshes_Addon.vdf");
    //m_FileIndex.loadVDF("vdf/Meshes.vdf");
    //m_FileIndex.loadVDF("vdf/Anims.vdf");
    //m_FileIndex.loadVDF("vdf/Anims_Addon.vdf");

    /*m_FileIndex.loadVDF("vdf/g1/anims.VDF");
	m_FileIndex.loadVDF("vdf/g1/fonts.VDF");
	m_FileIndex.loadVDF("vdf/g1/meshes.VDF");
	m_FileIndex.loadVDF("vdf/g1/sound_patch2.VDF");
	m_FileIndex.loadVDF("vdf/g1/sound.VDF");
	m_FileIndex.loadVDF("vdf/g1/speech_patch2.VDF");
	m_FileIndex.loadVDF("vdf/g1/speech.VDF");
	m_FileIndex.loadVDF("vdf/g1/textures_apostroph_patch_neu.VDF");
	m_FileIndex.loadVDF("vdf/g1/textures_choicebox_32pixel_modialpha.VDF");
	m_FileIndex.loadVDF("vdf/g1/textures_patch.VDF");
	m_FileIndex.loadVDF("vdf/g1/textures_Startscreen_ohne_Logo.VDF");
	m_FileIndex.loadVDF("vdf/g1/textures.VDF");
	m_FileIndex.loadVDF("vdf/g1/worlds.VDF");*/

    std::list<std::string> vdfArchives = Utils::getFilesInDirectory(m_Args.gameBaseDirectory + "/Data", "vdf");

    LogInfo() << "Loading VDF-Archives: " << vdfArchives;
    for (std::string& s : vdfArchives)
    {
        m_FileIndex.loadVDF(s);
    }

    // Happens on modded games
    std::list<std::string> vdfArchivesDisabled = Utils::getFilesInDirectory(m_Args.gameBaseDirectory + "/Data", "disabled");

    LogInfo() << "Loading VDF-Archives: " << vdfArchivesDisabled;
    for (std::string& s : vdfArchivesDisabled)
    {
        m_FileIndex.loadVDF(s);
    }

    // Load mod archives with higher priority
    std::list<std::string> modArchives = Utils::getFilesInDirectory(m_Args.gameBaseDirectory + "/Data", "mod", false);

    if (!modArchives.empty())
    {
        LogInfo() << "Loading MOD-Archives: " << modArchives;
        for (std::string& s : modArchives)
        {
            m_FileIndex.loadVDF(s, 1);
        }
    }

    // Load explicit modfile with even higher priority
    if (!m_Args.modfile.empty())
    {
        m_FileIndex.loadVDF(m_Args.modfile, 2);
    }
}

void BaseEngine::onWorldCreated(Handle::WorldHandle world)
{
}

World::WorldInstance& BaseEngine::getWorldInstance(Handle::WorldHandle& h)
{
    return h.get();
}

BaseEngine::EngineArgs BaseEngine::getEngineArgs()
{
    return m_Args;
}

bool BaseEngine::saveWorld(Handle::WorldHandle world, const std::string& file)
{
    json j;
    world.get().exportWorld(j);

    // Save
    std::ofstream f(file);
    if (!f.is_open())
        return false;

    f << Utils::iso_8859_1_to_utf8(j.dump(4));
    f.close();

    return true;
}

void BaseEngine::setPaused(bool paused)
{
    if (paused != m_Paused)
    {
        if (getMainWorld().isValid())
        {
            if (paused)
                getMainWorld().get().getAudioWorld().pauseSounds();
            else
                getMainWorld().get().getAudioWorld().continueSounds();
        }
        m_Paused = paused;
    }
}

void BaseEngine::queueSaveGameAction(SavegameManager::SaveGameAction saveGameAction)
{
    m_SaveGameActionQueue.push(saveGameAction);
}

void BaseEngine::processSaveGameActionQueue()
{
    while (!m_SaveGameActionQueue.empty())
    {
        SavegameManager::SaveGameAction action = m_SaveGameActionQueue.front();
        switch (action.type)
        {
            case SavegameManager::Save:
                if (!getMainWorld().get().getDialogManager().isDialogActive())
                {
                    // only save while not in Dialog
                    SavegameManager::saveToSlot(action.slot, action.savegameName);
                }
                break;
        }
        m_SaveGameActionQueue.pop();
    }
}

void BaseEngine::processMessageQueue()
{
    m_MessageQueueMutex.lock();
    auto current = m_MessageQueue.begin();
    while (current != m_MessageQueue.end())
    {
        auto& action = *current;
        // if the job queues a new job it would create a deadlock, so we need to release the mutex before
        m_MessageQueueMutex.unlock();
        bool finished = action.run(*this);
        m_MessageQueueMutex.lock();
        if (finished)
            current = m_MessageQueue.erase(current);  // erase returns next iterator
        else
            std::advance(current, 1);
    }
    m_MessageQueueMutex.unlock();
}

void BaseEngine::resetSession()
{
    // GameSession's destructor will clean up worlds
    m_Session = std::make_unique<GameSession>(*this);
}

GameClock& BaseEngine::getGameClock()
{
    return getSession().getGameClock();
}

Handle::WorldHandle BaseEngine::getMainWorld()
{
    return getSession().getMainWorld();
}

void BaseEngine::executeInMainThread(const AsyncAction::JobType<void>& job, bool forceQueue)
{
    auto wrappedJob = [job](Engine::BaseEngine* engine) -> bool {
        job(engine);
        return true;
    };
    executeInMainThreadUntilTrue(wrappedJob, forceQueue);
}

void BaseEngine::executeInMainThreadUntilTrue(const AsyncAction::JobType<bool>& job, bool forceQueue)
{
    if (!forceQueue && isMainThread())
    {
        // execute right away
        bool success = job(this);
        if (success)
            return;
        // else job returned false -> queue the job
    }
    std::lock_guard<std::mutex> guard(m_MessageQueueMutex);
    m_MessageQueue.emplace_back(AsyncAction{job});
}

bool BaseEngine::isMainThread()
{
    return std::this_thread::get_id() == m_MainThreadID;
}

size_t ExcludeFrameTime::m_ReferenceCounter = 0;