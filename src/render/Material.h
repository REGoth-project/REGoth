//
// Created by andre on 14.05.18.
//

#pragma once

#include <handle/HandleDef.h>
#include <content/Texture.h>
#include <optional.hpp>
#include <math/mathlib.h>

namespace Render
{
    namespace Content
    {
        struct Texture
        {
            Texture(Textures::TextureAllocator &allocator,
                    Handle::TextureHandle handle = Handle::TextureHandle::makeInvalidHandle())
                    : pAllocator(&allocator),
                      handle(handle)
            {}

            Handle::TextureHandle handle;
            std::string debugTag; // ie. file-name

            tl::optional<Textures::Texture &> get()
            {
                if (handle.isValid())
                {
                    return pAllocator->getTexture(handle);
                } else
                {
                    return tl::nullopt;
                }
            }

        private:
            Textures::TextureAllocator *pAllocator;
        };

        struct MeshMaterial
        {
            tl::optional<Texture> diffuseTexture;
            tl::optional<Texture> lightmapTexture;
            Math::float2 diffusePanningSpeed;
        };
    }
};
