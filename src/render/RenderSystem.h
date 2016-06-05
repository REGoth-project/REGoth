#pragma once

#include <bgfx/bgfx.h>

namespace Render
{
    struct RenderConfig
    {
        struct
        {
            bgfx::ProgramHandle mainWorldProgram;
        }programs;

        struct
        {
            bgfx::UniformHandle diffuseTexture;
        }uniforms;
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
        const RenderConfig& getConfig(){ return m_Config; }
    protected:

        /**
         * Generated config of this system
         */
        RenderConfig m_Config;
    };
}