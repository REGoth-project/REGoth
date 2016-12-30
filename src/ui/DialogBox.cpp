//
// Created by desktop on 11.08.16.
//

#include <imgui/imgui.h>
#include <render/RenderSystem.h>
#include <utils/logger.h>
#include "DialogBox.h"
#include <engine/BaseEngine.h>
#include "zFont.h"

using namespace UI;

DialogBox::DialogBox(Engine::BaseEngine& e) : View(e)
{
    m_ChoiceTaken = -1;
    m_ScrollArea = 0;
    m_CurrentlySelected = 0;
    m_BackgroundTexture = e.getEngineTextureAlloc().loadTextureVDF("DLG_CHOICE.TGA");

    setSize(Math::float2(0.75f, 1.0f));
    setTranslation(Math::float2(0.125f, 0));
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
    /*int height = config.state.viewHeight / 5;

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

    imguiEndScrollArea();*/

    const UI::zFont* fnt = m_Engine.getFontCache().getFont(DEFAULT_FONT);

    if(!fnt)
        return;

    // Un-normalize transforms
    Math::float2 absTranslation = getAbsoluteTranslation();
    Math::float2 absSize = getAbsoluteSize();

    int sx = (int) (absSize.x * config.state.viewWidth + 0.5f); // Span whole viewport
    int sy = fnt->getFontHeight() * std::max(8, (int)m_Choices.size()); // Scale with number of choices


    int px = (int) (absTranslation.x * config.state.viewWidth + 0.5f);
    int py = (int) (config.state.viewHeight - sy);

    // Draw background image
    {
        // Get background image
        Textures::Texture& background = m_Engine.getEngineTextureAlloc().getTexture(m_BackgroundTexture);

        bgfx::ProgramHandle program = config.programs.imageProgram;
        drawTexture(BGFX_VIEW, px, py, sx, sy,
                    config.state.viewWidth, config.state.viewHeight, background.m_TextureHandle, program,
                    config.uniforms.diffuseTexture);

    }

    // Draw text
    {
        float margin = 0.01f;
        int pMarginx = (int)(absSize.x * margin * config.state.viewWidth + 0.5f);
        int pMarginy = (int)(absSize.x * margin * config.state.viewHeight + 0.5f);
        for(int i=0;i<m_Choices.size();i++)
        {
            drawText(m_Choices[i].text,
                     px + pMarginx, py + fnt->getFontHeight() * i + pMarginy,
                     A_TopLeft,
                     config,
                     m_CurrentlySelected == i ? DEFAULT_FONT_HI : DEFAULT_FONT); // Highlight, if selected
        }
    }
}

size_t DialogBox::addChoice(Logic::DialogManager::ChoiceEntry& entry)
{
    if(m_CurrentlySelected == -1)
        m_CurrentlySelected = 0;

    m_Choices.push_back(entry);
    return m_Choices.size() - 1;
}

void DialogBox::clearChoices()
{
    m_Choices.clear();
    m_CurrentlySelected = -1;
    m_ChoiceTaken = -1;
}

void DialogBox::onInputAction(EInputAction action)
{
    if(m_Choices.empty())
        return;

    switch(action)
    {
        case IA_Up: m_CurrentlySelected = Utils::mod(m_CurrentlySelected - 1, (int)m_Choices.size()); break;
        case IA_Down: m_CurrentlySelected = Utils::mod(m_CurrentlySelected + 1, (int)m_Choices.size()); break;
        case IA_Left:break;
        case IA_Right:break;
        case IA_Close: m_ChoiceTaken = (int)m_Choices.size() - 1; break;
        case IA_Accept: m_ChoiceTaken = m_CurrentlySelected; break;
    }
}

