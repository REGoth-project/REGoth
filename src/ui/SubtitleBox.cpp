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
    if(m_IsHidden)
        return;

    View::update(dt, mstate, config);

    if(!m_Text.empty())
    {
        // Draw our choices
        const int height = config.state.viewHeight / 10;
        const int halfwidth = config.state.viewHeight / 2;
        imguiBeginScrollArea(m_Text.front().speaker.c_str(), (config.state.viewWidth / 2) - halfwidth, 10, halfwidth * 2, height,
                             &m_ScrollArea);

        imguiLabel(m_Text.front().text.c_str());

        imguiEndScrollArea();
    }

#define SINGLE_ACTION_KEY(key, fn) { \
    static bool last = false; \
    if(inputGetKeyState(key) && !last)\
        last = true; \
    else if(!inputGetKeyState(key) && last){\
        last = false;\
        fn();\
    } }

    // FIXME: Actually wait for the text and animation to finish
    // Go to next text if Return was pressed
    SINGLE_ACTION_KEY(entry::Key::KeyR, [&](){
        if(!m_Text.empty())
            m_Text.pop_front();
    });
}

void UI::SubtitleBox::addText(const std::string& speaker, const std::string& text)
{
    Text t;
    t.text = text;
    t.speaker = speaker;
    m_Text.push_back(t);
}
