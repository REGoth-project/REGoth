#include <zenload/zTypes.h>
#include "RenderSystem.h"
#include "bgfx_utils.h"
#include "utils/Utils.h"
#include <engine/BaseEngine.h>

using namespace Render;

RenderSystem::RenderSystem(Engine::BaseEngine& engine) : m_Engine(engine)
{

}

RenderSystem::~RenderSystem()
{
    // Clean the created resources
    bgfx::destroyProgram(m_Config.programs.mainWorldProgram);
    bgfx::destroyUniform(m_Config.uniforms.diffuseTexture);
}

void RenderSystem::init()
{
    m_Config.programs.mainWorldProgram = Utils::loadProgram(m_Engine.getContentBasePath().c_str(), "vs_world", "fs_stencil_texture_clip");
    m_Config.programs.mainWorldInstancedProgram = Utils::loadProgram(m_Engine.getContentBasePath().c_str(), "vs_world_instanced", "fs_stencil_texture_clip");
    m_Config.programs.mainSkinnedMeshProgram = Utils::loadProgram(m_Engine.getContentBasePath().c_str(), "vs_skinned", "fs_stencil_texture_clip");

    m_Config.programs.fullscreenQuadProgram = Utils::loadProgram(m_Engine.getContentBasePath().c_str(), "vs_screenquad", "fs_screenquad");

    m_Config.uniforms.diffuseTexture = bgfx::createUniform("s_texColor", bgfx::UniformType::Int1);
    m_Config.uniforms.objectColor = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
    m_Config.uniforms.nodeTransforms = bgfx::createUniform("PI_NodeTransforms", bgfx::UniformType::Vec4, 4 * ZenLoad::MAX_NUM_SKELETAL_NODES);

    // Sky/Fog
    m_Config.uniforms.skyCLUT = bgfx::createUniform("SKY_CLUT", bgfx::UniformType::Vec4, 256);
    m_Config.uniforms.fogColor = bgfx::createUniform("u_FogColor", bgfx::UniformType::Vec4);
    m_Config.uniforms.fogNearFar = bgfx::createUniform("u_FogNearFar", bgfx::UniformType::Vec4);
}

uint32_t RenderSystem::requestInstanceDataBuffer()
{
    // Check for a free spot
    if(!m_FreeInstanceDataBuffers.empty())
    {
        uint32_t p = m_FreeInstanceDataBuffers.back();
        m_FreeInstanceDataBuffers.pop_back();

        return p;
    }

    // TODO: Move this so we can have more than that
    bgfx::VertexDecl decl;
    decl.begin();
    decl.add(bgfx::Attrib::TexCoord1, 4, bgfx::AttribType::Float);
    decl.add(bgfx::Attrib::TexCoord2, 4, bgfx::AttribType::Float);
    decl.add(bgfx::Attrib::TexCoord3, 4, bgfx::AttribType::Float);
    decl.add(bgfx::Attrib::TexCoord4, 4, bgfx::AttribType::Float);
    decl.add(bgfx::Attrib::TexCoord5, 4, bgfx::AttribType::Float);
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


