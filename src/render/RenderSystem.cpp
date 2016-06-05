#include "RenderSystem.h"
#include "bgfx_utils.h"

using namespace Render;

RenderSystem::RenderSystem()
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
    m_Config.programs.mainWorldProgram = loadProgram("vs_stencil_texture", "fs_stencil_texture_clip");
    m_Config.uniforms.diffuseTexture = bgfx::createUniform("s_texColor", bgfx::UniformType::Int1);
}

