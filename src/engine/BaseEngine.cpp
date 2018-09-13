#include "BaseEngine.h"
#include <fstream>
#include "World.h"
#include "audio/AudioEngine.h"
#include "audio/NullAudioEngine.h"
#ifdef RE_USE_SOUND
#   include "audio/OpenALAudioEngine.h"
#endif
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
    Cli::Flag playerScriptname("p", "player", 1, "When starting a new game, the player will be inserted as the given NPC", {"PC_HERO"});
    Cli::Flag startNewGame("", "skipmenu", 0, "Skips the menu and starts a new game directly on game startup");

#ifdef RE_USE_SOUND
    Cli::Flag disableSound("", "no-sound", 0, "Disables audio output");
    Cli::Flag sndDevice("snd", "sound-device", 1, "OpenAL sound device", {""}, "Sound");
#endif

    Cli::Flag noTextureFiltering("nf", "disable-filtering", 0, "Disables texture filtering");
}

BaseEngine::BaseEngine()
    : m_JobManager(this)
    , m_RootUIView(*this)
    , m_Console(*this)
    , m_EngineTextureAlloc(*this)
{
    // m_JobManager.setMultiThreading(false); // useful for debugging exceptions from other threads
    m_pHUD = nullptr;
    m_pFontCache = nullptr;

    m_BasicGameType = Daedalus::GameType::GT_Gothic2;
    m_Paused = false;
    // allocate and init default session
    resetSession();
}

BaseEngine::~BaseEngine()
{
    getRootUIView().removeChild(m_pHUD);
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

    #ifdef RE_USE_SOUND
    std::string snd_device;
    if (Flags::sndDevice.isSet())
        snd_device = Flags::sndDevice.getParam(0);

    m_Args.noTextureFiltering = Flags::noTextureFiltering.isSet();

    if(Flags::disableSound.isSet())
    {
        m_AudioEngine = std::make_shared<Audio::NullAudioEngine>();
    }
    else
    {
        try
        {
            m_AudioEngine = std::make_shared<Audio::OpenALAudioEngine>(snd_device);
        }
        catch(const std::runtime_error& err)
        {
            LogError() << "Cannot initialize OpenAL audio engine: " << err.what();
            m_AudioEngine = std::make_shared<Audio::NullAudioEngine>();
        }
    }
    #else
    m_AudioEngine = std::make_shared<Audio::NullAudioEngine>();
    #endif

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
    // Load explicit modfile with highest priority
    if (!m_Args.modfile.empty())
    {
        LogInfo() << "Reading Mod-File from Commandline: " << m_Args.modfile;
        m_FileIndex.loadVDF(m_Args.modfile);
    }

    // Load mod archives
    std::list<std::string> modArchives = Utils::getFilesInDirectory(m_Args.gameBaseDirectory + "/Data", "mod", false);
    modArchives.sort([](const std::string &lhs, const std::string &rhs)
    { return VDFS::FileIndex::getLastModTime(lhs) > VDFS::FileIndex::getLastModTime(rhs); });
    LogInfo() << "Loading MOD-Archives: " << modArchives;
    if (!modArchives.empty())
        for (std::string& s : modArchives)
            m_FileIndex.loadVDF(s);

    // Load zip archives
    std::list<std::string> zipArchives = Utils::getFilesInDirectory(m_Args.gameBaseDirectory + "/Data", "zip", false);
    zipArchives.sort([](const std::string &lhs, const std::string &rhs)
    { return VDFS::FileIndex::getLastModTime(lhs) > VDFS::FileIndex::getLastModTime(rhs); });
    LogInfo() << "Loading ZIP-Archives: " << zipArchives;
    if (!zipArchives.empty())
        for (std::string& s : zipArchives)
            m_FileIndex.loadVDF(s);

    // Load vdf archives
    std::list<std::string> vdfArchives = Utils::getFilesInDirectory(m_Args.gameBaseDirectory + "/Data", "vdf");
    vdfArchives.sort([](const std::string &lhs, const std::string &rhs)
    { return VDFS::FileIndex::getLastModTime(lhs) > VDFS::FileIndex::getLastModTime(rhs); });
    LogInfo() << "Loading VDF-Archives: " << vdfArchives;
    for (std::string& s : vdfArchives)
        m_FileIndex.loadVDF(s);

    m_FileIndex.finalizeLoad();
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

void BaseEngine::resetSession()
{
    // the order is important: first destroy old session
    // GameSession's destructor will clean up worlds
    m_Session = nullptr;
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

JobManager& BaseEngine::getJobManager()
{
    return m_JobManager;
}

