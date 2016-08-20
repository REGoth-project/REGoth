#include <zenload/zTypes.h>
#include "RenderSystem.h"
#include "bgfx_utils.h"
#include "utils/Utils.h"

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
    m_Config.programs.mainWorldProgram = Utils::loadProgram("vs_stencil_texture", "fs_stencil_texture_clip");
    m_Config.programs.mainSkinnedMeshProgram = Utils::loadProgram("vs_skinned", "fs_stencil_texture_clip");

    m_Config.programs.fullscreenQuadProgram = Utils::loadProgram("vs_screenquad", "fs_screenquad");

    m_Config.uniforms.diffuseTexture = bgfx::createUniform("s_texColor", bgfx::UniformType::Int1);
    m_Config.uniforms.objectColor = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
    m_Config.uniforms.nodeTransforms = bgfx::createUniform("PI_NodeTransforms", bgfx::UniformType::Vec4, 4 * ZenLoad::MAX_NUM_SKELETAL_NODES);
    m_Config.uniforms.skyCLUT = bgfx::createUniform("SKY_CLUT", bgfx::UniformType::Vec4, 256);
}

