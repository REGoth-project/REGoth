#pragma once
#include "View.h"
#include <content/Texture.h>

namespace UI
{
    class TextView : public View
    {
    public:
        TextView(Engine::BaseEngine& e);
        ~TextView() override = default;

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        /**
         * Sets the string to display here. Can contain newlines!
         * @param text Text to display
         */
        void setText(const std::string& text) { m_Text = text; }
        /**
         * @param font Sets the font to use.
         */
        void setFont(const std::string& font) { m_Font = font; }

    protected:
        /**
         * String to display
         */
        std::string m_Text;

        /**
         * Font to use
         */
        std::string m_Font;
    };
}
