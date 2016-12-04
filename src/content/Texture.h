#pragma once
#include <handle/Handle.h>
#include <handle/HandleDef.h>
#include <map>
#include <vector>
#include <string>
#include "memory/Config.h"

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
    };

    typedef _Texture<Handle::InternalTextureHandle> Texture;

	class TextureAllocator
	{
	public:
		TextureAllocator(const VDFS::FileIndex* vdfidx = nullptr);
		virtual ~TextureAllocator();


		/**
		 * @brief Sets the VDFS-Index to use
		 */
		void setVDFSIndex(const VDFS::FileIndex* vdfidx) { m_pVDFSIndex = vdfidx; }

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
	protected:
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
		 * Pointer to a vdfs-index to work on (can be nullptr)
		 */
		const VDFS::FileIndex* m_pVDFSIndex;
	};

}