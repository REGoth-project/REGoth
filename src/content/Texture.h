#pragma once
#include <handle/Handle.h>
#include <handle/HandleDef.h>
#include <map>
#include <vector>
#include <string>
#include "memory/Config.h"

namespace Engine
{
	class BaseEngine;
}

namespace VDFS
{
	class FileIndex;
}

namespace Textures
{
    template<typename THDL>
    struct _Texture : public Handle::HandleTypeDescriptor<Handle::TextureHandle>
    {
		std::string m_TextureName;
        THDL m_TextureHandle;
        uint32_t m_Width;
        uint32_t m_Height;
		std::vector<uint8_t> imageData;
		bgfx::TextureFormat::Enum textureFormat;
    };

    typedef _Texture<Handle::InternalTextureHandle> Texture;

	class TextureAllocator
	{
	public:
		TextureAllocator(Engine::BaseEngine& engine);
		virtual ~TextureAllocator();


		/**
		 * @brief Loads a texture from the given DDS-Data
		 */
		Handle::TextureHandle loadTextureDDS(const std::vector<uint8_t>& data, const std::string& name = "");
		Handle::TextureHandle loadTextureRGBA8(const std::vector<uint8_t>& data, uint16_t width, uint16_t height, const std::string& name = "");

		/**
		 * @brief Loads a ZTEX-texture from the given or stored VDFS-FileIndex
		 */
		Handle::TextureHandle loadTextureVDF(const VDFS::FileIndex& idx, const std::string& name);
		Handle::TextureHandle loadTextureVDF(const std::string& name);

		/**
		 * @brief Returns the texture of the given handle
		 */
		Texture& getTexture(Handle::TextureHandle h) { return m_Allocator.getElement(h); }

		/**
		 * @return Rough estimation about how much memory the loaded textures need on the GPU in bytes
		 */
		size_t getEstimatedGPUMemoryConsumption() { return m_EstimatedGPUBytes; }
	protected:

		/**
		 * Pushes the loaded data to the GPU. Needs to run on the main-thread.
		 * @param h Data to finalize
		 * @return True if successful, false otherwise
		 */
		bool finalizeLoad(Handle::TextureHandle h);

		/**
		 * @brief Textures by their set names. Note: If names are doubled, only the last loaded texture
		 *		  can be found here
		 */
		std::map<std::string, Handle::TextureHandle> m_TexturesByName;

		/**
		 * Data allocator
		 */
		Memory::StaticReferencedAllocator<Textures::Texture, Config::MAX_NUM_LEVEL_TEXTURES> m_Allocator;

		/**
		 * Engine
		 */
		Engine::BaseEngine& m_Engine;

		/**
         * Rough estimation about how much memory the loaded textures need on the GPU in bytes
         */
		size_t m_EstimatedGPUBytes = 0;
	};

}