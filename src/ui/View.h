#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <common.h>
#include <engine/Input.h>
#include <render/RenderSystem.h>
#include <render/ViewList.h>

namespace Engine
{
    class BaseEngine;
}

namespace UI
{
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

        /**
         * copying forbidden
         */
        View(const View&) = delete;
        View& operator=(const View&) = delete;

        virtual ~View();

        /**
         * Adds a subview to this view.
         * Note: Pointer only stored for usage, freeing the memory is up to the caller.
         *       All children have to be removed from this, before this can be deleted, to ensure no memory leaks.
         * @param view View to add
         */
        void addChild(View* pView);

        /**
         * Removes a subview from this view
         * @param pView View to remove
         */
        void removeChild(View* pView);

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        virtual void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config);

        /**
         * Set/get hidden
         */
        bool isHidden() { return m_IsHidden; }
        void setHidden(bool v) { m_IsHidden = v; }

        /**
         * Transforms in normalized coords
         */
        void setTranslation(const Math::float2& translation) { m_Translation = translation; }
        void setSize(const Math::float2& size) { m_Size = size; }

        /**
         * Sets how this view should be aligned to it's translation
         * @param alignment The alignment
         */
        void setAlignment(EAlign alignment) { m_Alignment = alignment; }

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
        void drawText(const std::string& txt, int px, int py, EAlign alignment, Render::RenderConfig& config, const std::string& font = DEFAULT_FONT);

        // TODO document what intensity describes
        using ActionCallback = std::function<void(float /*intensity*/)>;
        using TextCallback = std::function<void(const std::string&)>;
        void registerActionHandler(Engine::ActionType actionType, ActionCallback actionCallback)
        {
            m_ActionCallbacks.emplace(actionType, std::move(actionCallback));
        }

        void registerTextHandler(TextCallback textCallback)
        {
            m_TextCallbacks.push_back(std::move(textCallback));
        }

        /**
         * @return whether the action was consumed and may not be passed to further views
         */
        bool onInputAction(Engine::ActionType actionType, float intensity)
        {
            bool childConsumedInput = std::any_of(m_Children.rbegin(), m_Children.rend(), [=](View* pChild){
                return pChild->onInputAction(actionType, intensity);
            });
            return childConsumedInput || consumesAction(actionType, intensity);
        }

        /**
         * @return whether the text was consumed and may not be passed to further views
         */
        bool onTextInput(const std::string& text)
        {
            bool childConsumedInput = std::any_of(m_Children.rbegin(), m_Children.rend(), [=](View* pChild){
                return pChild->onTextInput(text);
            });
            return childConsumedInput || consumesText(text);
        }

    protected:

        /**
         * @return true when you want to signal the parent view, that this action shall not be passed to further views
         */
        virtual bool consumesAction(Engine::ActionType actionType, float intensity)
        {
            if (isHidden())
                return false;
            fireBindings(actionType, intensity);
            return true;
        }

        /**
         * @return true when you want to signal the parent view, that this text shall not be passed to further views
         */
        virtual bool consumesText(const std::string& text)
        {
            if (isHidden())
                return false;
            fireBindings(text);
            return true;
        }

        /**
         * @brief calls all functions, that have been registered for the specified action
         */
        bool fireBindings(Engine::ActionType actionType, float intensity)
        {
            bool fired = false;
            // is it save to store the iterators? iterator invalidation through callback?
            auto rangeIterators = m_ActionCallbacks.equal_range(actionType);
            for (auto itAction = rangeIterators.first; itAction != rangeIterators.second; ++itAction)
            {
                bool enabled = true; // TODO use struct with bool enabled
                if (enabled)
                {
                    itAction->second(intensity);
                }
                fired = true;
            }
            return fired;
        }

        /**
         * @brief calls all functions, that have been registered
         */
        bool fireBindings(const std::string& text)
        {
            bool fired = !m_TextCallbacks.empty();
            for (auto& callback : m_TextCallbacks)
                callback(text);
            return fired;
        }

        /**
         * @param pParent Parent of this view
         */
        void setParent(View* pParent) { m_pParent = pParent; };

        /**
         * Sub-views drawn on top of this one
         */
        std::vector<View*> m_Children;
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

        std::multimap<Engine::ActionType, ActionCallback> m_ActionCallbacks;
        std::vector<TextCallback> m_TextCallbacks;
    };
}
