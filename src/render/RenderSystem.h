#pragma once

#include <bgfx/bgfx.h>
#include <math/mathlib.h>
#include <vector>

namespace Engine
{
class BaseEngine;
}

namespace Render
{
struct RenderConfig
{
    struct
    {
        bgfx::ProgramHandle mainWorldProgram;
        bgfx::ProgramHandle mainWorldInstancedProgram;
        bgfx::ProgramHandle mainSkinnedMeshProgram;
        bgfx::ProgramHandle fullscreenQuadProgram;
        bgfx::ProgramHandle imageProgram;
        bgfx::ProgramHandle particle_textured;
    } programs;

    struct
    {
        bgfx::UniformHandle diffuseTexture;
        bgfx::UniformHandle nodeTransforms;
        bgfx::UniformHandle objectColor;
        bgfx::UniformHandle skyCLUT;
        bgfx::UniformHandle fogColor;
        bgfx::UniformHandle fogNearFar;
        bgfx::UniformHandle s_TexColor;
    } uniforms;

    struct
    {
        Math::Matrix cameraWorld;
        float drawDistanceSquared;
        float farPlane;
        uint32_t viewWidth;
        uint32_t viewHeight;
    } state;
};

class RenderSystem
{
public:
    RenderSystem(Engine::BaseEngine& engine);
    virtual ~RenderSystem();

    /**
         * @Initializes the system and creates the resources depending on the subclass-type
         */
    virtual void init();

    /**
         * @return The generated config of this system
         */
    RenderConfig& getConfig() { return m_Config; }
    /**
         * @return Free value to use as instance-data-buffer. This gives you a place to store
         *         bgfx::InstanceDataBuffers by index. Note that only the spot is reserved.
         *         Note: Unregister this when you don't need it anymore!
         */
    uint32_t requestInstanceDataBuffer();

    /**
         * Frees the spot of the instance-data-buffer at the given index
         * @param idx Index to mark as free
         */
    void unregisterInstanceDataBuffer(uint32_t idx);

    /**
         * Access to the stored instanceDataBuffer at the given index
         * @param idx Index to look at
         * @return InstanceDataBuffer at the given index. If this is non-nullptr, you got valid data here.
         */
    inline bgfx::DynamicVertexBufferHandle getFrameInstanceDataBuffer(uint32_t idx)
    {
        return m_InstanceDataBuffers[idx];
    }

protected:
    /**
         * Generated config of this system
         */
    RenderConfig m_Config;

    Engine::BaseEngine& m_Engine;

    /**
         * Instance buffers used at render-time
         */
    std::vector<bgfx::DynamicVertexBufferHandle> m_InstanceDataBuffers;
    std::vector<uint32_t> m_FreeInstanceDataBuffers;
};
}