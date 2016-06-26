#pragma once

#include <bgfx/bgfx.h>
#include <math/mathlib.h>

namespace Render
{
    struct RenderConfig
    {
        struct
        {
            bgfx::ProgramHandle mainWorldProgram;
            bgfx::ProgramHandle mainSkinnedMeshProgram;
        }programs;

        struct
        {
            bgfx::UniformHandle diffuseTexture;
            bgfx::UniformHandle nodeTransforms;
        }uniforms;

        struct
        {
            Math::Matrix cameraWorld;
            float drawDistanceSquared;
        }state;
    };

    class RenderSystem
    {
    public:

        RenderSystem();
        virtual ~RenderSystem();

        /**
         * @Initializes the system and creates the resources depending on the subclass-type
         */
        virtual void init();

        /**
         * @return The generated config of this system
         */
        RenderConfig& getConfig(){ return m_Config; }
    protected:

        /**
         * Generated config of this system
         */
        RenderConfig m_Config;
    };
}