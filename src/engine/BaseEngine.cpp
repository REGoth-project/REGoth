#include "audio/AudioEngine.h"
#include "BaseEngine.h"
#include <vdfs/fileIndex.h>
#include <zenload/zenParser.h>
#include <zenload/zCMesh.h>
#include <components/EntityActions.h>
#include <utils/bgfx_lib.h>
#include "World.h"
#include <render/WorldRender.h>
#include <utils/logger.h>
#include <bx/commandline.h>
#include <zenload/zCModelPrototype.h>
#include <components/Vob.h>
#include <fstream>
#include <ui/Hud.h>
#include <ui/zFont.h>
#include <utils/cli.h>

using namespace Engine;

namespace Flags
{
    Cli::Flag gameDirectory("g", "game-dir", 1, "Root-folder of your Gothic installation");
    Cli::Flag modFile("m", "mod-file", 1, "Additional .mod-file to load");
    Cli::Flag world("w", "world", 1, ".ZEN-file to load out of one of the vdf-archives");
    Cli::Flag sndDevice("snd", "sound-device", 1, "OpenAL sound device");
}

BaseEngine::BaseEngine() : m_RootUIView(*this)
{
    m_pHUD = nullptr;
    m_pFontCache = nullptr;
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
    //m_Args.startupZEN = "addonworld.zen";

    if(Flags::gameDirectory.isSet())
        m_Args.gameBaseDirectory = Flags::gameDirectory.getArgs()[0];
    else
        LogInfo() << "No game-root specified! Using the current working-directory as game root. Use the '-g' flag to specify this!";

    if(Flags::modFile.isSet())
        m_Args.modfile = Flags::modFile.getArgs()[0];

    if(Flags::world.isSet())
        m_Args.startupZEN = Flags::world.getArgs()[0];


    loadArchives();

    if(m_Args.startupZEN.empty() || !m_FileIndex.hasFile(m_Args.startupZEN))
    {
        // Try Gothic 1
        if(m_FileIndex.hasFile("world.zen"))
            m_Args.startupZEN = "world.zen";
        else if(m_FileIndex.hasFile("newworld.zen"))
            m_Args.startupZEN = "newworld.zen";
        else
            LogWarn() << "Unknown game files, could not find world.zen or newworld.zen!";
    }

    std::string snd_device;
    if(Flags::sndDevice.isSet())
        snd_device = Flags::sndDevice.getArgs()[0];

    m_AudioEngine = new Audio::AudioEngine(snd_device);

    // Init HUD
    m_pFontCache = new UI::zFontCache(*this);
    m_pHUD = new UI::Hud(*this);
    getRootUIView().addChild(m_pHUD);
}

Handle::WorldHandle  BaseEngine::addWorld(const std::string & _worldFile, const std::string& savegame)
{
    std::string worldFile = _worldFile;

    m_WorldInstances.emplace_back();

	World::WorldInstance& world = m_WorldInstances.back();
	onWorldCreated(world.getMyHandle());

    // Try to load a savegame
    json savegameData;
    if(!savegame.empty())
    {
        std::ifstream f(savegame);
        std::stringstream saveData;
        saveData << f.rdbuf();

        savegameData = json::parse(saveData);
        worldFile = savegameData["zenfile"];
    }

    if(!worldFile.empty())
    {
        std::vector<uint8_t> zenData;
        m_FileIndex.getFileData(worldFile, zenData);

        if (zenData.empty())
        {
            LogWarn() << "Failed to find world file: " << worldFile;
            return Handle::WorldHandle::makeInvalidHandle();
        }
    }

    if (!world.init(*this, worldFile, savegameData))
    {
        LogError() << "Failed to init world file: " << worldFile;
        return Handle::WorldHandle::makeInvalidHandle();
    }

    if(!m_Args.testVisual.empty())
    {
        LogInfo() << "Testing visual: " << m_Args.testVisual;
        Handle::EntityHandle e = Vob::constructVob(world);
        Vob::VobInformation vob = Vob::asVob(world, e);

        Vob::setVisual(vob, m_Args.testVisual);
    }

	m_Worlds.push_back(world.getMyHandle());

	return world.getMyHandle();
}

void BaseEngine::removeWorld(Handle::WorldHandle world)
{
    std::remove(m_Worlds.begin(), m_Worlds.end(), world);

    for(auto it = m_WorldInstances.begin(); it != m_WorldInstances.end(); it++)
    {
        if(&(*it) == &world.get())
        {
            m_WorldInstances.erase(it);
            break;
        }
    }
}

void BaseEngine::frameUpdate(double dt, uint16_t width, uint16_t height)
{
	onFrameUpdate(dt, width, height);

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
    for(std::string& s : vdfArchives)
    {
        m_FileIndex.loadVDF(s);
    }

    // Happens on modded games
    std::list<std::string> vdfArchivesDisabled = Utils::getFilesInDirectory(m_Args.gameBaseDirectory + "/Data", "disabled");

    LogInfo() << "Loading VDF-Archives: " << vdfArchivesDisabled;
    for(std::string& s : vdfArchivesDisabled)
    {
        m_FileIndex.loadVDF(s);
    }


	// Load mod archives with higher priority
    std::list<std::string> modArchives = Utils::getFilesInDirectory(m_Args.gameBaseDirectory + "/Data", "mod", false);

    if(!modArchives.empty())
    {
        LogInfo() << "Loading MOD-Archives: " << modArchives;
        for (std::string &s : modArchives)
        {
            m_FileIndex.loadVDF(s, 1);
        }
    }

    // Load explicit modfile with even higher priority
    if(!m_Args.modfile.empty())
    {
    	m_FileIndex.loadVDF(m_Args.modfile, 2);
    }

    // Init global texture alloc
    m_EngineTextureAlloc.setVDFSIndex(&m_FileIndex);
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


Handle::WorldHandle BaseEngine::loadWorld(const std::string& worldFile, const std::string& savegame)
{
    Engine::Input::clearActions(); // FIXME: This should be taken care of by the objects having something bound

    while(!m_WorldInstances.empty())
    {
        Handle::WorldHandle w = m_Worlds.front();
        removeWorld(w);
    }

    return addWorld(worldFile, savegame); 
}

void BaseEngine::setMainWorld(Handle::WorldHandle world)
{
    m_MainWorld = world;
}


bool BaseEngine::saveWorld(Handle::WorldHandle world, const std::string& file)
{
    json j;
    world.get().exportWorld(j);

    // Save
    std::ofstream f(file);
    if(!f.is_open())
        return false;

    f << Utils::iso_8859_1_to_utf8(j.dump(4));
    f.close();

    return true;
}
