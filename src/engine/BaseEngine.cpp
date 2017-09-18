#include "BaseEngine.h"
#include "GameSession.h"
#include <fstream>
#include "World.h"
#include <bx/commandline.h>
#include <components/EntityActions.h>
#include <components/Vob.h>
#include <components/VobClasses.h>
#include <logic/PlayerController.h>
#include <render/WorldRender.h>
#include <utils/bgfx_lib.h>
#include <utils/logger.h>
#include <vdfs/fileIndex.h>
#include <zenload/zCMesh.h>
#include <zenload/zCModelPrototype.h>
#include <zenload/zenParser.h>

using namespace Engine;

BaseEngine::BaseEngine()
{
    m_BasicGameType = Daedalus::GameType::GT_Gothic2;
    m_Paused = false;
    m_ExcludedFrameTime = 0;
    // allocate and init default session
}

BaseEngine::~BaseEngine()
{
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

size_t ExcludeFrameTime::m_ReferenceCounter = 0;
