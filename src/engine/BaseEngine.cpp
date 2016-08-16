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

using namespace Engine;

BaseEngine::BaseEngine()
{

}

BaseEngine::~BaseEngine()
{
}

void BaseEngine::initEngine(int argc, char** argv)
{
    bx::CommandLine cmdLine(argc, (const char**)argv);
    const char* value = nullptr;

    m_Args.gameBaseDirectory = ".";
    //m_Args.startupZEN = "addonworld.zen";

    if(cmdLine.hasArg('g'))
    {
        value = cmdLine.findOption('g');

        if(value)
        {
            m_Args.gameBaseDirectory = value;
            LogInfo() << "Using " << m_Args.gameBaseDirectory << " as game root";
        }
    } else
    {
        LogInfo() << "No game-root specified! Using the current working-directory as game root. Use the '-g' flag to specify this!";
    }

    if(cmdLine.hasArg('m'))
    {
        value = cmdLine.findOption('m');

        if(value)
        {
            m_Args.modfile = value;
            LogInfo() << "Using modfile " << m_Args.modfile;
        }
    }

    if(cmdLine.hasArg('w'))
    {
        value = cmdLine.findOption('w');

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
}

Handle::WorldHandle  BaseEngine::addWorld(const std::string & worldFile)
{
	World::WorldInstance::HandleType w = m_WorldInstances.createObject();
	World::WorldInstance& world = m_WorldInstances.getElement(w);
	onWorldCreated(w);

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

    world.init(*this, worldFile);

    if(!m_Args.testVisual.empty())
    {
        LogInfo() << "Testing visual: " << m_Args.testVisual;
        Handle::EntityHandle e = Vob::constructVob(world);
        Vob::VobInformation vob = Vob::asVob(world, e);

        Vob::setVisual(vob, m_Args.testVisual);
    }

	m_Worlds.push_back(w);



	return w;
}

Handle::WorldHandle  BaseEngine::addWorld()
{
	World::WorldInstance::HandleType w = m_WorldInstances.createObject();
	World::WorldInstance& world = m_WorldInstances.getElement(w);

	world.init(*this);
	m_Worlds.push_back(w);

	onWorldCreated(w);

	return w;
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
	m_WorldInstances.getElement(world).setMyHandle(world);
}

World::WorldInstance& BaseEngine::getWorldInstance(Handle::WorldHandle& h)
{
	return m_WorldInstances.getElement(h);
}

BaseEngine::EngineArgs BaseEngine::getEngineArgs()
{
    return m_Args;
}






