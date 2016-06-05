#include "Texture.h"
#include <bgfx/bgfx.h>
#include <vdfs/fileIndex.h>
#include <zenload/ztex2dds.h>
#include <utils/logger.h>

using namespace Textures;

TextureAllocator::TextureAllocator(const VDFS::FileIndex* vdfidx)
	: m_pVDFSIndex(vdfidx)
{
}

TextureAllocator::~TextureAllocator()
{
	// Delete all textures
	for (size_t i = 0; i < m_Allocator.getNumObtainedElements(); i++)
	{
		bgfx::TextureHandle h = m_Allocator.getElements()[i].m_TextureHandle;
		bgfx::destroyTexture(h);
	}
}

Handle::TextureHandle TextureAllocator::loadTextureDDS(const std::vector<uint8_t>& data, const std::string & name)
{
	// Check if this was already loaded
	auto it = m_TexturesByName.find(name);
	if (it != m_TexturesByName.end())
		return (*it).second;

	// Try to load the texture first, so we don't have to clean up if this fails
	//TODO: Avoid the second copy here
	const bgfx::Memory* mem = bgfx::alloc(data.size());
	memcpy(mem->data, data.data(), data.size());
	bgfx::TextureHandle bth = bgfx::createTexture(mem);

	// Couldn't load this one?
	if (bth.idx == bgfx::invalidHandle)
		return Handle::TextureHandle::makeInvalidHandle();

	// Make wrapper-object
	Handle::TextureHandle h = m_Allocator.createObject();

	m_Allocator.getElement(h).m_TextureHandle = bth;
	m_Allocator.getElement(h).m_TextureName = name;

	// Add handle to name-map, if it got one
	if(!name.empty())
		m_TexturesByName[name] = h;

	// Flush the pipeline
	// TODO: There must be something better than "frame"?
	//bgfx::touch(0);
	//bgfx::frame();

	return h;
}

Handle::TextureHandle TextureAllocator::loadTextureVDF(const VDFS::FileIndex & idx, const std::string & name)
{
	// Check if this was already loaded
	auto it = m_TexturesByName.find(name);
	if (it != m_TexturesByName.end())
		return (*it).second;

	std::string vname = name;
	std::vector<uint8_t> ztex;
	std::vector<uint8_t> dds;

	// Check if this isn't the compiled version
	if (vname.find("-C") == std::string::npos)
	{
		// Strip the ".TGA"
		vname = vname.substr(0, vname.size() - 4);

		// Add "compiled"-extension
		vname += "-C.TEX";
	}

	// Load from archive
	idx.getFileData(vname, ztex);

	// Failed?
	if (ztex.empty())
		return Handle::TextureHandle::makeInvalidHandle();

	// Convert to usual DDS
	ZenLoad::convertZTEX2DDS(ztex, dds);

	// Proceed to load as usual dds-file and the input-name
	return loadTextureDDS(dds, name);
}

Handle::TextureHandle TextureAllocator::loadTextureVDF(const std::string & name)
{
	if (!m_pVDFSIndex)
		return Handle::TextureHandle::makeInvalidHandle();

	return loadTextureVDF(*m_pVDFSIndex, name);
}
