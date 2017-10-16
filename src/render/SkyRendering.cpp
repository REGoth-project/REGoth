//
// Created by desktop on 15.10.17.
//

#include "SkyRendering.h"
#include "RenderSystem.h"
#include <content/Sky.h>
#include <engine/BaseEngine.h>
#include <engine/World.h>

static bool isSkyDomeAvailable(const Content::Sky& sky);
static void drawSkyDomeOf(World::WorldInstance& world, const Render::RenderConfig& renderConfig);
static void drawSkyPlaneOf(World::WorldInstance& world, const Render::RenderConfig& renderConfig);
static void drawDomeColorLayerOf(World::WorldInstance& world, const Render::RenderConfig& renderConfig);
static void drawDomeCloudsLayerOf(World::WorldInstance& world, const Render::RenderConfig& renderConfig);


void Render::Sky::drawSkyOf(World::WorldInstance& world, const RenderConfig& renderConfig)
{
    if(isSkyDomeAvailable(world.getSky()))
    {
        drawSkyDomeOf(world, renderConfig);
    }
    else
    {
        drawSkyPlaneOf(world, renderConfig);
    }
}


static bool isSkyDomeAvailable(const Content::Sky& sky)
{
    return sky.getSkyDomeColorLayerMesh().isValid();
}


static void drawSkyDomeOf(World::WorldInstance& world, const Render::RenderConfig& renderConfig)
{
    drawDomeColorLayerOf(world, renderConfig);
    drawDomeCloudsLayerOf(world, renderConfig);

}


static void drawDomeColorLayerOf(World::WorldInstance& world, const Render::RenderConfig& renderConfig)
{
    Handle::MeshHandle colorLayer = world.getSky().getSkyDomeColorLayerMesh();

    if (!colorLayer.isValid())
        return;

    const Meshes::WorldStaticMesh& mesh = world.getStaticMeshAllocator().getMesh(colorLayer);

    for (size_t i = 0; i < mesh.mesh.m_SubmeshStarts.size(); i++)
    {
        Math::Matrix skyTransform = Math::Matrix::CreateTranslation(renderConfig.state.cameraWorld.Translation());
        skyTransform = skyTransform * Math::Matrix::CreateTranslation(0, -5, 0);
        bgfx::setTransform(skyTransform.m);


        bgfx::setVertexBuffer(mesh.mesh.m_VertexBufferHandle);
        bgfx::setIndexBuffer(mesh.mesh.m_IndexBufferHandle,
                             mesh.mesh.m_SubmeshStarts[i].m_StartIndex,
                             mesh.mesh.m_SubmeshStarts[i].m_NumIndices);


        Math::float3 interpColor0, interpColor1;
        world.getSky().getDomeColors(interpColor0, interpColor1);

        Math::float4 skyColorsVec4[2] = {
                Math::float4(interpColor0.x, interpColor0.y, interpColor0.z, 1.0f),
                Math::float4(interpColor1.x, interpColor1.y, interpColor1.z, 1.0f),
        };

        bgfx::setUniform(renderConfig.uniforms.skyColors, (float*) skyColorsVec4, 2);

        Math::float4 color = {1.0f, 1.0f, 1.0f, 1.0f};
        bgfx::setUniform(renderConfig.uniforms.objectColor, color.v);

        uint64_t state = BGFX_STATE_DEFAULT;
        state |= BGFX_STATE_BLEND_ADD;
        state &= ~BGFX_STATE_DEPTH_WRITE;
        bgfx::setState(state);

        bgfx::submit(RenderViewList::PRE_WORLD_SKY, renderConfig.programs.skyDomeColorProgram);
    }
}


static void drawDomeCloudsLayerOf(World::WorldInstance& world, const Render::RenderConfig& renderConfig)
{
    const float totalSecondsRealtime = (float)world.getEngine()->getGameClock().getTotalSecondsRealtime();

    const auto& domeLayers = world.getSky().getDomeMeshes();

    for(size_t l = 0 ; l < domeLayers.size(); l++)
    {
        Handle::MeshHandle domeLayer = domeLayers[l];

        if (!domeLayer.isValid())
            continue;

        const Meshes::WorldStaticMesh& mesh = world.getStaticMeshAllocator().getMesh(domeLayer);
        const auto& layer = world.getSky().getMasterState().layers[l];

        for (size_t i = 0; i < mesh.mesh.m_SubmeshStarts.size(); i++)
        {
            Handle::TextureHandle textureHandle = mesh.mesh.m_SubmeshMaterials[i].m_TextureHandle;

            if (!textureHandle.isValid())
                continue;

            Textures::Texture& texture = world.getTextureAllocator().getTexture(textureHandle);
            bgfx::setTexture(0, renderConfig.uniforms.diffuseTexture, texture.m_TextureHandle);

            Math::Matrix skyTransform = Math::Matrix::CreateTranslation(renderConfig.state.cameraWorld.Translation());
            bgfx::setTransform(skyTransform.m);


            bgfx::setVertexBuffer(mesh.mesh.m_VertexBufferHandle);
            bgfx::setIndexBuffer(mesh.mesh.m_IndexBufferHandle,
                                 mesh.mesh.m_SubmeshStarts[i].m_StartIndex,
                                 mesh.mesh.m_SubmeshStarts[i].m_NumIndices);


            Math::float4 color = {1.0f, 1.0f, 1.0f, layer.textureAlpha};
            bgfx::setUniform(renderConfig.uniforms.objectColor, color.v);


            Math::float2 texPositionOffset = 0.01f * layer.textureSpeed * totalSecondsRealtime;
            
            if(world.getSky().isNightTime()) // Stars don't move as much
                texPositionOffset = { 0.0f, 0.0f };

            Math::float4 skyTextureParams = Math::float4(layer.textureScale, layer.textureScale,
                                                         texPositionOffset.x, texPositionOffset.y);
            bgfx::setUniform(renderConfig.uniforms.skyTextureParams, skyTextureParams.v);

            uint64_t state = BGFX_STATE_DEFAULT;
            state |= BGFX_STATE_BLEND_ALPHA;
            state &= ~BGFX_STATE_DEPTH_WRITE;
            bgfx::setState(state);

            Math::float4 skyColors[2];
            world.getSky().getSkyColors(skyColors[0], skyColors[1]);
            bgfx::setUniform(renderConfig.uniforms.skyColors, (float*)skyColors, 2);

            bgfx::submit(RenderViewList::PRE_WORLD_SKY, renderConfig.programs.skyProgram);
        }
    }
}


static void drawSkyPlaneOf(World::WorldInstance& world, const Render::RenderConfig& renderConfig)
{
    const auto& planeLayers = world.getSky().getSkyPlaneMeshes();
    const float totalSecondsRealtime = (float)world.getEngine()->getGameClock().getTotalSecondsRealtime();

    for(size_t l = 0 ; l < planeLayers.size(); l++)
    {
        Handle::MeshHandle planeLayer = planeLayers[l];

        if (!planeLayer.isValid())
            continue;

        const Meshes::WorldStaticMesh& mesh = world.getStaticMeshAllocator().getMesh(planeLayer);
        const auto& layer = world.getSky().getMasterState().layers[l];

        for (size_t i = 0; i < mesh.mesh.m_SubmeshStarts.size(); i++)
        {
            Handle::TextureHandle textureHandle = mesh.mesh.m_SubmeshMaterials[i].m_TextureHandle;

            if (!textureHandle.isValid())
                continue;

            Textures::Texture& texture = world.getTextureAllocator().getTexture(textureHandle);
            bgfx::setTexture(0, renderConfig.uniforms.diffuseTexture, texture.m_TextureHandle);

            Math::Matrix skyTransform = Math::Matrix::CreateTranslation(renderConfig.state.cameraWorld.Translation());
            bgfx::setTransform(skyTransform.m);


            bgfx::setVertexBuffer(mesh.mesh.m_VertexBufferHandle);
            bgfx::setIndexBuffer(mesh.mesh.m_IndexBufferHandle,
                                 mesh.mesh.m_SubmeshStarts[i].m_StartIndex,
                                 mesh.mesh.m_SubmeshStarts[i].m_NumIndices);

            Math::float3 polyColor;
            if(l == 1)
                polyColor = world.getSky().getPolyCloudsLayerColor();
            else
                polyColor = {1.0f, 1.0f, 1.0f};

            Math::float4 color = {polyColor.x, polyColor.y, polyColor.z, layer.textureAlpha};
            bgfx::setUniform(renderConfig.uniforms.objectColor, color.v);

            Math::float2 texPositionOffset = 0.01f * layer.textureSpeed * totalSecondsRealtime;

            if(world.getSky().isNightTime()) // Stars don't move as much
                texPositionOffset = { 0.0f, 0.0f };

            Math::float4 skyTextureParams = Math::float4(20.0f * layer.textureScale, 20.0f * layer.textureScale,
                                                         texPositionOffset.x, texPositionOffset.y);
            bgfx::setUniform(renderConfig.uniforms.skyTextureParams, skyTextureParams.v);

            uint64_t state = BGFX_STATE_DEFAULT;
            state |= BGFX_STATE_BLEND_ALPHA;
            state &= ~BGFX_STATE_DEPTH_WRITE;
            bgfx::setState(state);

            bgfx::submit(RenderViewList::PRE_WORLD_SKY, renderConfig.programs.skyProgram);
        }
    }
}