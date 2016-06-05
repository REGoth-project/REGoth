#include "BaseEngine.h"
#include <vdfs/fileIndex.h>
#include <zenload/zenParser.h>
#include <zenload/zCMesh.h>
#include <components/EntityActions.h>
#include <utils/bgfx_lib.h>
#include "World.h"
#include <render/WorldRender.h>
#include <utils/logger.h>

using namespace Engine;

BaseEngine::BaseEngine()
{
	
}

BaseEngine::~BaseEngine()
{
}

void BaseEngine::initEngine()
{
	loadArchives();
}

Handle::WorldHandle  BaseEngine::addWorld(const std::string & worldFile)
{
	World::WorldInstance::HandleType w = m_WorldInstances.createObject();
	World::WorldInstance& world = m_WorldInstances.getElement(w);

	std::vector<uint8_t> zenData;
	m_FileIndex.getFileData(worldFile, zenData);

	if (zenData.empty())
	{
		LogWarn() << "Failed to find world file: " << worldFile;
		return Handle::WorldHandle::makeInvalidHandle();
	}

	world.init(*this, worldFile);

	m_Worlds.push_back(w);

	onWorldCreated(w);

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

void BaseEngine::frameUpdate(double dt)
{
	onFrameUpdate(dt);

}

void BaseEngine::loadArchives()
{
	m_FileIndex.loadVDF("vdf/Worlds_Addon.vdf");
	m_FileIndex.loadVDF("vdf/Textures.vdf");
	m_FileIndex.loadVDF("vdf/Textures_Addon.vdf");
	m_FileIndex.loadVDF("vdf/Meshes_Addon.vdf");
	m_FileIndex.loadVDF("vdf/Meshes.vdf");
	m_FileIndex.loadVDF("vdf/Anims.vdf");
	m_FileIndex.loadVDF("vdf/Anims_Addon.vdf");

	//m_FileIndex.loadVDF("vdf/g1/anims.VDF");
	//m_FileIndex.loadVDF("vdf/g1/fonts.VDF");
	//m_FileIndex.loadVDF("vdf/g1/meshes.VDF");
	//m_FileIndex.loadVDF("vdf/g1/sound_patch2.VDF");
	//m_FileIndex.loadVDF("vdf/g1/sound.VDF");
	//m_FileIndex.loadVDF("vdf/g1/speech_patch2.VDF");
	//m_FileIndex.loadVDF("vdf/g1/speech.VDF");
	//m_FileIndex.loadVDF("vdf/g1/textures_apostroph_patch_neu.VDF");
	//m_FileIndex.loadVDF("vdf/g1/textures_choicebox_32pixel_modialpha.VDF");
	//m_FileIndex.loadVDF("vdf/g1/textures_patch.VDF");
	//m_FileIndex.loadVDF("vdf/g1/textures_Startscreen_ohne_Logo.VDF");
	//m_FileIndex.loadVDF("vdf/g1/textures.VDF");
	//m_FileIndex.loadVDF("vdf/g1/worlds.VDF");
}

void BaseEngine::onWorldCreated(Handle::WorldHandle world)
{
	m_WorldInstances.getElement(world).setMyHandle(world);
}






