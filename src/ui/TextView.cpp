//
// Created by desktop on 19.12.16.
//

#include "TextView.h"

UI::TextView::TextView(Engine::BaseEngine& e) :
        View(e),
        m_Font("FONT_OLD_20_WHITE.FNT")
{
    setAlignment(A_Center);
}

UI::TextView::~TextView()
{

}

void UI::TextView::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if(m_IsHidden)
        return;


    // Get position of the text
    Math::float2 absTranslation = getAbsoluteTranslation();
    int px = (int) (absTranslation.x * config.state.viewWidth + 0.5f);
    int py = (int) (absTranslation.y * config.state.viewHeight + 0.5f);

    // Draw text to screen. Alignment is calculated internally
    drawText(m_Text, px, py, m_Alignment, config, m_Font);

    View::update(dt, mstate, config);
}
