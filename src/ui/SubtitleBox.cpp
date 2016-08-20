//
// Created by desktop on 11.08.16.
//

#include <imgui/imgui.h>
#include <render/RenderSystem.h>
#include <entry/input.h>
#include <utils/logger.h>
#include "SubtitleBox.h"

UI::SubtitleBox::SubtitleBox()
{
    m_ScrollArea = 0;
}

UI::SubtitleBox::~SubtitleBox()
{

}

void UI::SubtitleBox::update(double dt, entry::MouseState& mstate, Render::RenderConfig& config)
{
    if (m_IsHidden)
        return;

    View::update(dt, mstate, config);


    // Draw our choices
    const int height = config.state.viewHeight / 10;
    const int halfwidth = config.state.viewHeight / 2;
    imguiBeginScrollArea(m_Text.speaker.c_str(), (config.state.viewWidth / 2) - halfwidth, 10, halfwidth * 2, height,
                         &m_ScrollArea);

    imguiLabel(m_Text.text.c_str());

    imguiEndScrollArea();



}

void UI::SubtitleBox::setText(const std::string& speaker, const std::string& text)
{
    m_Text.text = text;
    m_Text.speaker = speaker;
}
