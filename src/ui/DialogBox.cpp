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
}

DialogBox::~DialogBox()
{

}

void DialogBox::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if(m_IsHidden)
        return;

    View::update(dt, mstate, config);

    // Draw our choices
    int height = config.state.viewHeight / 5;

#ifdef BX_PLATFORM_ANDROID
    height *= 2;
#endif

    imguiBeginScrollArea(m_TargetName.c_str(), 10, config.state.viewHeight - height - 10, config.state.viewWidth - 20, height, &m_ScrollArea);

    for(size_t i=0;i<m_Choices.size();i++)
    {
        if(imguiButton(m_Choices[i].text.c_str()))
        {
            m_ChoiceTaken = static_cast<int>(i);
        }
    }

    imguiEndScrollArea();
}

size_t DialogBox::addChoice(Logic::DialogManager::ChoiceEntry& entry)
{
    m_Choices.push_back(entry);
    return m_Choices.size() - 1;
}

