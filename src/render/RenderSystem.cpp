#include "RenderSystem.h"
#include "bgfx_utils.h"
#include "utils/Utils.h"
#include <engine/BaseEngine.h>
#include <utils/logger.h>
#include <zenload/zTypes.h>
#include <content/VertexTypes.h>
#include <content/StaticMeshAllocator.h>

#if BX_PLATFORM_EMSCRIPTEN
#include <emscripten.h>
#include <html5.h>
#endif

using namespace Render;

RenderSystem::RenderSystem(Engine::BaseEngine& engine)
    : m_Engine(engine)
{
}

RenderSystem::~RenderSystem()
{
    // Clean the created resources
    for(bgfx::ProgramHandle h : m_LoadedPrograms)
        bgfx::destroyProgram(h);

    for(bgfx::UniformHandle h : m_AllUniforms)
        bgfx::destroyUniform(h);

    for(bgfx::DynamicVertexBufferHandle h : m_InstanceDataBuffers)
        bgfx::destroyDynamicVertexBuffer(h);
}

void RenderSystem::init()
{
    m_Config.programs.mainWorldProgram = Utils::loadProgram(m_Engine.getContentBasePath().c_str(), "vs_world", "fs_stencil_texture_clip");
    m_LoadedPrograms.push_back(m_Config.programs.mainWorldProgram);

    m_Config.programs.mainWorldInstancedProgram = Utils::loadProgram(m_Engine.getContentBasePath().c_str(), "vs_world_instanced", "fs_stencil_texture_clip");
    m_LoadedPrograms.push_back(m_Config.programs.mainWorldInstancedProgram);

    m_Config.programs.mainSkinnedMeshProgram = Utils::loadProgram(m_Engine.getContentBasePath().c_str(), "vs_skinned", "fs_stencil_texture_clip");
    m_LoadedPrograms.push_back(m_Config.programs.mainSkinnedMeshProgram);

    m_Config.programs.particle_textured = Utils::loadProgram(m_Engine.getContentBasePath().c_str(), "vs_particle", "fs_particle_textured");
    m_LoadedPrograms.push_back(m_Config.programs.particle_textured);

    m_Config.programs.fullscreenQuadProgram = Utils::loadProgram(m_Engine.getContentBasePath().c_str(), "vs_screenquad", "fs_screenquad");
    m_LoadedPrograms.push_back(m_Config.programs.fullscreenQuadProgram);

    m_Config.programs.imageProgram = Utils::loadProgram(m_Engine.getContentBasePath().c_str(), "vs_image", "fs_image");
    m_LoadedPrograms.push_back(m_Config.programs.imageProgram);

    m_Config.uniforms.diffuseTexture = bgfx::createUniform("s_texColor", bgfx::UniformType::Int1);
    m_AllUniforms.push_back(m_Config.uniforms.diffuseTexture);

    m_Config.uniforms.objectColor = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
    m_AllUniforms.push_back(m_Config.uniforms.objectColor);

    m_Config.uniforms.nodeTransforms = bgfx::createUniform("PI_NodeTransforms", bgfx::UniformType::Vec4, 4 * ZenLoad::MAX_NUM_SKELETAL_NODES);
    m_AllUniforms.push_back(m_Config.uniforms.nodeTransforms);


    // Sky/Fog
    m_Config.uniforms.skyCLUT = bgfx::createUniform("SKY_CLUT", bgfx::UniformType::Vec4, 256);
    m_AllUniforms.push_back(m_Config.uniforms.skyCLUT);

    m_Config.uniforms.skyColors = bgfx::createUniform("u_SkyColors", bgfx::UniformType::Vec4, 2);
    m_AllUniforms.push_back(m_Config.uniforms.skyColors);

    m_Config.uniforms.fogColor = bgfx::createUniform("u_FogColor", bgfx::UniformType::Vec4);
    m_AllUniforms.push_back(m_Config.uniforms.fogColor);

    m_Config.uniforms.fogNearFar = bgfx::createUniform("u_FogNearFar", bgfx::UniformType::Vec4);
    m_AllUniforms.push_back(m_Config.uniforms.fogNearFar);


#if BX_PLATFORM_EMSCRIPTEN
    int enabled = emscripten_webgl_enable_extension(emscripten_webgl_get_current_context(), "OES_element_index_uint");
    assert(enabled == 1);
    LogInfo() << "OES_element_index_uint enabled: " << enabled;
#endif
}

uint32_t RenderSystem::requestInstanceDataBuffer()
{
    // Check for a free spot
    if (!m_FreeInstanceDataBuffers.empty())
    {
        uint32_t p = m_FreeInstanceDataBuffers.back();
        m_FreeInstanceDataBuffers.pop_back();

        return p;
    }

    // TODO: Move this so we can have more than that
    bgfx::VertexDecl decl;
    decl.begin();
    decl.add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float);
    decl.add(bgfx::Attrib::TexCoord1, 4, bgfx::AttribType::Float);
    decl.add(bgfx::Attrib::TexCoord2, 4, bgfx::AttribType::Float);
    decl.add(bgfx::Attrib::TexCoord3, 4, bgfx::AttribType::Float);
    decl.add(bgfx::Attrib::TexCoord4, 4, bgfx::AttribType::Float);
    decl.end();

    // Create a new spot
    m_InstanceDataBuffers.push_back(bgfx::createDynamicVertexBuffer(1, decl, BGFX_BUFFER_ALLOW_RESIZE));
    return (uint32_t)m_InstanceDataBuffers.size() - 1;
}

void RenderSystem::unregisterInstanceDataBuffer(uint32_t idx)
{
    // Mark spot as free
    m_FreeInstanceDataBuffers.push_back(idx);
}

void screenSpaceQuad(float _textureWidth, float _textureHeight, float _width = 1.0f, float _height = 1.0f)
{
    if (bgfx::getAvailTransientVertexBuffer(3, Meshes::WorldStaticMeshVertex::ms_decl))
    {
        const bgfx::Caps* caps = bgfx::getCaps();
        const bgfx::RendererType::Enum renderer = bgfx::getRendererType();
        float _texelHalf = bgfx::RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;

        bgfx::TransientVertexBuffer vb;
        bgfx::allocTransientVertexBuffer(&vb, 3, Meshes::WorldStaticMeshVertex::ms_decl);
        Meshes::WorldStaticMeshVertex* vertex = (Meshes::WorldStaticMeshVertex*)vb.data;

        const float minx = -_width;
        const float maxx = _width;
        const float miny = 0.0f;
        const float maxy = _height * 2.0f;

        const float texelHalfW = _texelHalf / _textureWidth;
        const float texelHalfH = _texelHalf / _textureHeight;
        const float minu = -1.0f + texelHalfW;
        const float maxu = 1.0f + texelHalfH;

        const float zz = 0.0f;

        float minv = texelHalfH;
        float maxv = 2.0f + texelHalfH;

        if (caps->originBottomLeft)
        {
            float temp = minv;
            minv = maxv;
            maxv = temp;

            minv -= 1.0f;
            maxv -= 1.0f;
        }

        vertex[0].Position.x = minx;
        vertex[0].Position.y = miny;
        vertex[0].Position.z = zz;
        vertex[0].TexCoord.x = minu;
        vertex[0].TexCoord.y = minv;

        vertex[1].Position.x = maxx;
        vertex[1].Position.y = miny;
        vertex[1].Position.z = zz;
        vertex[1].TexCoord.x = maxu;
        vertex[1].TexCoord.y = minv;

        vertex[2].Position.x = maxx;
        vertex[2].Position.y = maxy;
        vertex[2].Position.z = zz;
        vertex[2].TexCoord.x = maxu;
        vertex[2].TexCoord.y = maxv;

        bgfx::setVertexBuffer(&vb);
    }
}
