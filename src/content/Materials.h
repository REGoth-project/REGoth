#pragma once
#include <handle/Handle.h>
#include <handle/HandleDef.h>
#include "Texture.h"

namespace Materials
{
    /**
     * Base material-structure
     * Defines handle-type and other needed properties
     */
    struct Material : public Handle::HandleTypeDescriptor<Handle::MaterialHandle>
    {

    };

    /**
     * Material featuring a diffuse-texture
     */
    struct TexturedMaterial : public Material
    {
        Textures::Texture::HandleType m_TextureHandle;
        std::string m_TextureName;
        bool m_NoCollision;
    };
};