//
// Created by desktop on 11.08.16.
//

#include <imgui/imgui.h>
#include <render/RenderSystem.h>
#include <utils/logger.h>
#include "DialogBox.h"

using namespace UI;

DialogBox::DialogBox()
{
    m_ChoiceTaken = -1;
    m_ScrollArea = 0;
    fillChoices({"Hi, ich bin neu hier.", "Du scheinst nicht von hier zu kommen?", "ENDE"}, "Melvin");
}

DialogBox::~DialogBox()
{

}

void DialogBox::update(double dt, entry::MouseState& mstate, Render::RenderConfig& config)
{
    if(m_IsHidden)
        return;

    View::update(dt, mstate, config);

    // Draw our choices
    const int height = config.state.viewHeight / 6;
    imguiBeginScrollArea(m_TargetName.c_str(), 10, config.state.viewHeight - height - 10, config.state.viewWidth - 20, height, &m_ScrollArea);

    for(size_t i=0;i<m_Choices.size();i++)
    {
        if(imguiButton(m_Choices[i].c_str()))
        {
            m_ChoiceTaken = static_cast<int>(i);
        }
    }

    imguiEndScrollArea();
}

void DialogBox::fillChoices(const std::vector<std::string>& choices, const std::string& name)
{
    m_Choices = choices;
    m_TargetName = name;
}

