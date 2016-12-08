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

using namespace Engine;

BaseEngine::BaseEngine()
{

}

BaseEngine::~BaseEngine()
{
}

void BaseEngine::initEngine(int argc, char** argv)
{
    m_Args.cmdline = bx::CommandLine(argc, (const char**)argv);
    const char* value = nullptr;

    LogInfo() << "Initializing...";

    for(int i=0;i<argc;i++)
    {
        LogInfo() << "Arg " << i;
        LogInfo() << " - " << argv[i];
    }


    m_Args.gameBaseDirectory = ".";
    //m_Args.startupZEN = "addonworld.zen";

    if(m_Args.cmdline.hasArg('g'))
    {
        value = m_Args.cmdline.findOption('g');

        if(value)
        {
            m_Args.gameBaseDirectory = value;
            LogInfo() << "Using " << m_Args.gameBaseDirectory << " as game root";
        }
    } else
    {
        LogInfo() << "No game-root specified! Using the current working-directory as game root. Use the '-g' flag to specify this!";
    }

    if(m_Args.cmdline.hasArg('m'))
    {
        value = m_Args.cmdline.findOption('m');

        if(value)
        {
            m_Args.modfile = value;
            LogInfo() << "Using modfile " << m_Args.modfile;
        }
    }

    if(m_Args.cmdline.hasArg('w'))
    {
        value = m_Args.cmdline.findOption('w');

        if(value)
        {
            m_Args.startupZEN = value;
            LogInfo() << "Loading world " << m_Args.startupZEN << " on startup";
        }
    } else
    {
        LogInfo() << "No startup-world specified. Using 'addonworld.zen' as default. Use the '-w' flag to specify a ZEN-file!";
    }


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
}

Handle::WorldHandle  BaseEngine::addWorld(const std::string & worldFile, const std::string& savegame)
{
    m_WorldInstances.emplace_back();

	World::WorldInstance& world = m_WorldInstances.back();
	onWorldCreated(world.getMyHandle());

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

    // Try to load a savegame
    json savegameData;
    if(!savegame.empty())
    {
        std::ifstream f(savegame);
        std::stringstream saveData;
        saveData << f.rdbuf();

        savegameData = json::parse(saveData);
    }

    world.init(*this, worldFile, savegameData);

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







