#include "Texture.h"
#include <bgfx/bgfx.h>
#include <engine/BaseEngine.h>
#include <utils/logger.h>
#include <vdfs/fileIndex.h>
#include <zenload/ztex2dds.h>

using namespace Textures;

// These are compiled inside bgfx
typedef unsigned char stbi_uc;
extern "C" stbi_uc* stbi_load_from_memory(stbi_uc const* _buffer, int _len, int* _x, int* _y, int* _comp, int _req_comp);
extern "C" void stbi_image_free(void* _ptr);

TextureAllocator::TextureAllocator(Engine::BaseEngine& engine)
    : m_Engine(engine)
{
}

TextureAllocator::~TextureAllocator()
{
    // Delete all textures
    for (size_t i = 0; i < m_Allocator.getNumObtainedElements(); i++)
    {
        bgfx::TextureHandle h = m_Allocator.getElements()[i].m_TextureHandle;
        bgfx::destroy(h);
    }

    m_EstimatedGPUBytes = 0;
}

Handle::TextureHandle TextureAllocator::loadTextureDDS(const std::vector<uint8_t>& data, const std::string& name)
{
    // Check if this was already loaded
    auto it = m_TexturesByName.find(name);
    if (it != m_TexturesByName.end())
        return (*it).second;

    // Make wrapper-object
    Handle::TextureHandle h = m_Allocator.createObject();

    m_Allocator.getElement(h).textureFormat = bgfx::TextureFormat::Unknown;
    m_Allocator.getElement(h).imageData = data;
    //m_Allocator.getElement(h).m_TextureHandle = bth;
    m_Allocator.getElement(h).m_TextureName = name;

    ZenLoad::DDSURFACEDESC2 desc = ZenLoad::getSurfaceDesc(data);
    m_Allocator.getElement(h).m_Width = desc.dwWidth;
    m_Allocator.getElement(h).m_Height = desc.dwHeight;

    // Add handle to name-map, if it got one
    if (!name.empty())
        m_TexturesByName[name] = h;

    // Flush the pipeline
    // TODO: There must be something better than "frame"?
    //bgfx::touch(0);
    //bgfx::frame();

    return h;
}

Handle::TextureHandle TextureAllocator::loadTextureRGBA8(const std::vector<uint8_t>& data, uint16_t width, uint16_t height, const std::string& name)
{
    // Check if this was already loaded
    auto it = m_TexturesByName.find(name);
    if (it != m_TexturesByName.end())
        return (*it).second;

    // Load image
    //int width, height, comp;
    //void* out = stbi_load_from_memory( data.data(), data.size(), (int*)&width, (int*)&height, &comp, 4);

    // Try to load the texture first, so we don't have to clean up if this fails
    //TODO: Avoid the second copy here

    // Make wrapper-object
    Handle::TextureHandle h = m_Allocator.createObject();

    m_Allocator.getElement(h).textureFormat = bgfx::TextureFormat::RGBA8;
    m_Allocator.getElement(h).imageData = data;
    //m_Allocator.getElement(h).m_TextureHandle = bth;
    m_Allocator.getElement(h).m_TextureName = name;
    m_Allocator.getElement(h).m_Width = width;
    m_Allocator.getElement(h).m_Height = height;

    // Add handle to name-map, if it got one
    if (!name.empty())
        m_TexturesByName[name] = h;

    // Flush the pipeline
    // TODO: There must be something better than "frame"?
    //bgfx::touch(0);
    //bgfx::frame();

    return h;
}

Handle::TextureHandle TextureAllocator::loadTextureVDF(const VDFS::FileIndex& idx, const std::string& name)
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
    bool asDDS = true;
    idx.getFileData(vname, ztex);

    // No compiled version? Try again as TGA
    if (ztex.empty())
    {
        vname = name;
        idx.getFileData(vname, ztex);
        asDDS = false;
    }

    // Failed?
    if (ztex.empty())
        return Handle::TextureHandle::makeInvalidHandle();

    if (asDDS)
    {
        // Convert to usual DDS
        ZenLoad::convertZTEX2DDS(ztex, dds);
    }

#if EMSCRIPTEN
    if (asDDS)
    {
        LogInfo() << "Converting DDS to RGBA8 for: " << name;
        // Android doesn't support DDS for the most part
        ztex.clear();
        ZenLoad::convertDDSToRGBA8(dds, ztex);
        asDDS = false;
    }
#endif

    Handle::TextureHandle h;
    if (asDDS)
    {
        // Proceed to load as usual dds-file and the input-name
        h = loadTextureDDS(dds, name);
    }
    else
    {
        ZenLoad::DDSURFACEDESC2 desc = ZenLoad::getSurfaceDesc(dds);
        h = loadTextureRGBA8(ztex, (uint16_t)desc.dwWidth, (uint16_t)desc.dwHeight, name);
    }

    m_Engine.executeInThread([this, h](Engine::BaseEngine* pEngine) {
        finalizeLoad(h);
    }, Engine::ExecutionPolicy::MainThread);

    return h;
}

Handle::TextureHandle TextureAllocator::loadTextureVDF(const std::string& name)
{
    return loadTextureVDF(m_Engine.getVDFSIndex(), name);
}

bool TextureAllocator::finalizeLoad(Handle::TextureHandle h)
{
    Texture& tx = m_Allocator.getElement(h);

    if (tx.textureFormat == bgfx::TextureFormat::RGBA8)
    {
        const bgfx::Memory* mem = bgfx::alloc(tx.imageData.size());
        memcpy(mem->data, tx.imageData.data(), tx.imageData.size());
        bgfx::TextureHandle bth = bgfx::createTexture2D(tx.m_Width, tx.m_Height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_NONE, mem);

        m_EstimatedGPUBytes += tx.imageData.size();

        // Free imange
        //stbi_image_free(out);

        // Couldn't load this one?
        if (bth.idx == bgfx::kInvalidHandle)
            return false;

        tx.m_TextureHandle = bth;
    }
    else
    {
        // Try to load the texture first, so we don't have to clean up if this fails
        //TODO: Avoid the second copy here
        const bgfx::Memory* mem = bgfx::alloc(tx.imageData.size());
        memcpy(mem->data, tx.imageData.data(), tx.imageData.size());
        bgfx::TextureHandle bth = bgfx::createTexture(mem);

        m_EstimatedGPUBytes += tx.imageData.size();

        // Couldn't load this one?
        if (bth.idx == bgfx::kInvalidHandle)
            return false;

        tx.m_TextureHandle = bth;
    }

    return true;
}
