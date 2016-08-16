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
        void update(double dt, entry::MouseState &mstate, Render::RenderConfig &config) override;

        /**
         * Sets the text to be displayed inside the box
         */
        void addText(const std::string& speaker, const std::string& text);

        /**
         * Whether this subtitle-box still has text left
         */
        bool hasText(){ return !m_Text.empty(); }
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
        };

        std::list<Text> m_Text;

        /**
         * Current state of the scroll-area
         */
        int32_t m_ScrollArea;
    };
}
