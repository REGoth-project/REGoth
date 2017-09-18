#pragma once

#include "View.h"
#include <handle/HandleDef.h>

namespace UI
{
    class SubtitleBox : public View
    {
    public:
        SubtitleBox(Engine::GameEngine& e);
        ~SubtitleBox();

        enum class TextAlignment
        {
            center,
            left
        };

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        /**
         * Sets the text to be displayed inside the box
         */
        void setText(const std::string& speaker, const std::string& text);

        void setGrowDirection(float direction) { m_growDirection = direction; }
        void setScaling(float scaling) { m_Scaling = scaling; }
        float getScaling() { return m_Scaling; }
    protected:
        struct Text
        {
            /**
             * Text to display inside the box
             */
            std::string text;

            /**
             * Text of the person speaking
             */
            std::string speaker;
        } m_Text;

        /**
         * Current state of the scroll-area
         */
        int32_t m_ScrollArea;

        /**
         * Current scaling
         */
        float m_Scaling;

        /**
         * Current grow direction
         */
        float m_growDirection;

        /**
         * Dark background behind the subtitles
         */
        Handle::TextureHandle m_BackgroundTexture;

        /**
         * Time it takes to grow from min scaling to max scaling in seconds
         */
        static constexpr float GROW_SHRINK_TIME = 0.26f;
    };
}
