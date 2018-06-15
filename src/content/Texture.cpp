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
    Handle::TextureHandle h = createEmptyTexture(name);

    fillTextureDDS(h, data);

    return h;
}

void TextureAllocator::fillTextureDDS(Handle::TextureHandle h, const std::vector<uint8_t>& data)
{
    m_Allocator.getElement(h).textureFormat = bgfx::TextureFormat::Unknown;
    m_Allocator.getElement(h).imageData = data;

    ZenLoad::DDSURFACEDESC2 desc = ZenLoad::getSurfaceDesc(data);
    m_Allocator.getElement(h).m_Width = desc.dwWidth;
    m_Allocator.getElement(h).m_Height = desc.dwHeight;
}


Handle::TextureHandle TextureAllocator::loadTextureRGBA8(const std::vector<uint8_t>& data, uint16_t width, uint16_t height, const std::string& name)
{
    // Check if this was already loaded
    auto it = m_TexturesByName.find(name);
    if (it != m_TexturesByName.end())
        return (*it).second;

    // Make wrapper-object
    Handle::TextureHandle h = createEmptyTexture(name);

    fillTextureRGBA8(h, data, width, height);

    return h;
}


void TextureAllocator::fillTextureRGBA8(Handle::TextureHandle h,
                                        const std::vector<uint8_t>& data,
                                        uint16_t width,
                                        uint16_t height)
{
    m_Allocator.getElement(h).textureFormat = bgfx::TextureFormat::RGBA8;
    m_Allocator.getElement(h).imageData = data;
    m_Allocator.getElement(h).m_Width = width;
    m_Allocator.getElement(h).m_Height = height;
}


Handle::TextureHandle TextureAllocator::createEmptyTexture(const std::string& name)
{
    // Check if this was already loaded
    auto it = m_TexturesByName.find(name);
    if (it != m_TexturesByName.end())
        return (*it).second;

    // Make wrapper-object
    Handle::TextureHandle h = m_Allocator.createObject();
    m_Allocator.getElement(h).m_TextureName = name;

    // Add handle to name-map, if it got one
    if (!name.empty())
        m_TexturesByName[name] = h;

    return h;
}


Handle::TextureHandle TextureAllocator::loadTextureVDF(const VDFS::FileIndex& idx, const std::string& name)
{
    // Check if this was already loaded
    auto it = m_TexturesByName.find(name);
    if (it != m_TexturesByName.end())
        return (*it).second;

    std::string vname = name;

    // Check if this isn't the compiled version
    if (vname.find("-C") == std::string::npos)
    {
        // Strip the ".TGA"
        vname = vname.substr(0, vname.size() - 4);

        // Add "compiled"-extension
        vname += "-C.TEX";
    }

    bool asDDS = true;
    if(!idx.hasFile(vname))
    {
        // No compiled version? Try raw...
        vname = name;
        asDDS = false;

        if(!idx.hasFile(vname))
            return Handle::TextureHandle::makeInvalidHandle();
    }

    Handle::TextureHandle h = createEmptyTexture(name);

    m_Engine.getJobManager().executeInThread<void>([=, &idx](Engine::BaseEngine* pEngine) {

        std::vector<uint8_t> ztex;
        std::vector<uint8_t> dds;

        // Load from archive
        idx.getFileData(vname, ztex);

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

        if (asDDS)
        {
            // Proceed to load as usual dds-file and the input-name
            fillTextureDDS(h, dds);
        }
        else
        {
            ZenLoad::DDSURFACEDESC2 desc = ZenLoad::getSurfaceDesc(dds);

            fillTextureRGBA8(h, ztex, (uint16_t)desc.dwWidth, (uint16_t)desc.dwHeight);
        }

        m_Engine.getJobManager().executeInMainThread<void>([this, h](Engine::BaseEngine* pEngine) {
            finalizeLoad(h);
        });

    }, Engine::ExecutionPolicy::NewThread);

    return h;
}

Handle::TextureHandle TextureAllocator::loadTextureVDF(const std::string& name)
{
    return loadTextureVDF(m_Engine.getVDFSIndex(), name);
}

bool TextureAllocator::finalizeLoad(Handle::TextureHandle h)
{
    Texture& tx = m_Allocator.getElement(h);

    std::uint32_t textureFlags = BGFX_TEXTURE_NONE;

    if (this->m_Engine.getEngineArgs().noTextureFiltering)
    {
        textureFlags = BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT | BGFX_TEXTURE_MIP_POINT;
    }

    if (tx.textureFormat == bgfx::TextureFormat::RGBA8)
    {
        const bgfx::Memory* mem = bgfx::alloc(tx.imageData.size());
        memcpy(mem->data, tx.imageData.data(), tx.imageData.size());
        bgfx::TextureHandle bth = bgfx::createTexture2D(tx.m_Width, tx.m_Height, false, 1, bgfx::TextureFormat::RGBA8, textureFlags, mem);

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
        bgfx::TextureHandle bth = bgfx::createTexture(mem, textureFlags);

        m_EstimatedGPUBytes += tx.imageData.size();

        // Couldn't load this one?
        if (bth.idx == bgfx::kInvalidHandle)
            return false;

        tx.m_TextureHandle = bth;
    }

    return true;
}
