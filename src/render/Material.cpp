//
// Created by andre on 14.05.18.
//

#include "Material.h"
#include "Render.h"
#include "RenderSystem.h"

static void bindAsDiffuseTexture(const Textures::Texture &texture)
{
    auto config = Render::getConfig();

    if(!config)
        return;

    bgfx::setTexture(0, config->uniforms.diffuseTexture, texture.m_TextureHandle);
}

void Render::Content::MeshMaterial::bind() const
{
    bgfx::setState(BGFX_STATE_DEFAULT);

    diffuseTexture.and_then(&Texture::get).map(&bindAsDiffuseTexture);

}
