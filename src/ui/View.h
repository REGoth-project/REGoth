#pragma once
#include <common.h>
#include <vector>
#include <render/RenderSystem.h>
#include <engine/Input.h>

namespace UI
{
    // BGFX-View to be used for rendering views
    const int BGFX_VIEW = 254;

    /**
     * Base UI-View
     */
    class View
    {
    public:
        View();

        virtual ~View();

        /**
         * Adds a subview to this view.
         * Note: Pointer only stored for usage, freeing the memory is up to the caller.
         *       All children have to be removed from this, before this can be deleted, to ensure no memory leaks.
         * @param view View to add
         */
        void addChild(View *pView);

        /**
         * Removes a subview from this view
         * @param pView View to remove
         */
        void removeChild(View *pView);

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        virtual void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config);

        /**
         * Set/get hidden
         */
        bool isHidden(){ return m_IsHidden; }
        void setHidden(bool v){ m_IsHidden = v; }

        /**
         * Transforms in normalized coords
         */
        void setTranslation(const Math::float2& translation){ m_Translation = translation; }
        void setSize(const Math::float2& size){ m_Size = size; }

        /**
         * Draws a texture on screen somewhere
         * Note: Uses alpha-blending
         * @param x/y/width/height Transforms in pixel-coords (topleft: 0,0)
         * @param texture Texture to draw
         */
        static void drawTexture(uint8_t id, int x, int y, int width, int height, int surfaceWidth, int surfaceHeight,
                                bgfx::TextureHandle texture, bgfx::ProgramHandle program, bgfx::UniformHandle texUniform);

        /**
         * @return The absolute position/size using the parents
         */
        Math::float2 getAbsoluteTranslation();
        Math::float2 getAbsoluteSize();

    protected:

        /**
         * @param pParent Parent of this view
         */
        void setParent(View* pParent){ m_pParent = pParent; };

        /**
         * Sub-views drawn on top of this one
         */
        std::vector<View *> m_Children;
        View* m_pParent;

        /**
         * Whether this should be drawn/processed
         */
        bool m_IsHidden;

        /**
         * Transforms
         */
        Math::float2 m_Translation;
        Math::float2 m_Size;
    };
}