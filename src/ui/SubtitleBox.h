#pragma once
#include "View.h"

namespace UI
{
    class SubtitleBox : public View
    {
    public:
        SubtitleBox();
        ~SubtitleBox();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config) override;

        /**
         * Sets the text to be displayed inside the box
         */
        void setText(const std::string& speaker, const std::string& text);

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
        }m_Text;

        /**
         * Current state of the scroll-area
         */
        int32_t m_ScrollArea;
    };
}
