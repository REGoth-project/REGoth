#pragma once
#include <common.h>
#include <vector>
#include <render/RenderSystem.h>
#include <engine/Input.h>
#include <render/ViewList.h>

namespace Engine
{
    class BaseEngine;
}

namespace UI
{
    enum EInputAction
    {
        IA_Up,
        IA_Down,
        IA_Left,
        IA_Right,
        IA_Close,
        IA_Accept,
	IA_Backspace
    };

    // BGFX-View to be used for rendering views
    const int BGFX_VIEW = RenderViewList::UI;

    // Default font to use when drawing text
    const char* const DEFAULT_FONT = "FONT_OLD_10_WHITE.FNT";
    const char* const DEFAULT_FONT_HI = "FONT_OLD_10_WHITE_HI.FNT";
    const char* const DEFAULT_FONT_LARGE = "FONT_OLD_20_WHITE.FNT";
    const char* const DEFAULT_FONT_LARGE_HI = "FONT_OLD_20_WHITE_HI.FNT";

    enum EAlign
    {
        A_Center,
        A_LeftCenter,
        A_RightCenter,
        A_TopCenter,
        A_BottomCenter,
        A_TopLeft,
        A_TopRight,
        A_BottomLeft,
        A_BottomRight
    };

    /**
     * Base UI-View
     */
    class View
    {
    public:
        View(Engine::BaseEngine& e);

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
         * Sets how this view should be aligned to it's translation
         * @param alignment The alignment
         */
        void setAlignment(EAlign alignment){ m_Alignment = alignment; }

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

        /**
         * From everything being aligned to "top-left" by default, we can realize other alignments by shifting the
         * top-left translation before rendering, which is what this function outputs.
         * @param align Alignment to account for
         * @param width Width of the area to align
         * @param height Height of the area to align
         * @return Offset to the top-left translation of a view
         */
        static Math::float2 getAlignOffset(EAlign align, float width, float height);

        /**
         * Draws the given lines of text (\n is allowed!) to the screen
         * @param txt Text to draw
         * @param px
         * @param py
         */
        void drawText(const std::string& txt, int px, int py, EAlign alignment, Render::RenderConfig &config, const std::string& font = DEFAULT_FONT);
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
        EAlign m_Alignment;

        /**
         * Engine
         */
         Engine::BaseEngine& m_Engine;
    };
}
