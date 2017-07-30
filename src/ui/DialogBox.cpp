//
// Created by desktop on 11.08.16.
//

#include "DialogBox.h"
#include "zFont.h"
#include <engine/BaseEngine.h>
#include <imgui/imgui.h>
#include <render/RenderSystem.h>
#include <utils/logger.h>

using namespace UI;

DialogBox::DialogBox(Engine::BaseEngine& e)
    : View(e)
{
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
    if (m_IsHidden)
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

    if (!fnt)
        return;

    // Un-normalize transforms
    Math::float2 absTranslation = getAbsoluteTranslation();
    Math::float2 absSize = getAbsoluteSize();

    int sx = Math::iround(absSize.x * config.state.viewWidth);               // Span whole viewport
    int sy = fnt->getFontHeight() * std::max(8, (int)m_Choices.size() + 1);  // Scale with number of choices +1 for some extra space

    int px = Math::iround(absTranslation.x * config.state.viewWidth);
    int py = config.state.viewHeight - sy;

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
        int pMarginx = Math::iround(absSize.x * margin * config.state.viewWidth);
        int pMarginy = Math::iround(absSize.x * margin * config.state.viewHeight);
        for (unsigned i = 0; i < m_Choices.size(); i++)
        {
            drawText(m_Choices[i].text,
                     px + pMarginx, py + fnt->getFontHeight() * i + pMarginy,
                     A_TopLeft,
                     config,
                     static_cast<unsigned>(m_CurrentlySelected) == i ? DEFAULT_FONT_HI : DEFAULT_FONT);  // Highlight, if selected
        }
    }
}

size_t DialogBox::addChoice(Logic::DialogManager::ChoiceEntry& entry)
{
    if (m_CurrentlySelected == -1)
        m_CurrentlySelected = 0;

    m_Choices.push_back(entry);
    return m_Choices.size() - 1;
}

void DialogBox::clearChoices()
{
    m_Choices.clear();
    m_CurrentlySelected = -1;
}

void DialogBox::onInputAction(EInputAction action)
{
    if (m_Choices.empty())
        return;

    switch (action)
    {
        case IA_Up:
            m_CurrentlySelected = Utils::mod(m_CurrentlySelected - 1, (int)m_Choices.size());
            break;
        case IA_Down:
            m_CurrentlySelected = Utils::mod(m_CurrentlySelected + 1, (int)m_Choices.size());
            break;
        case IA_Left:
            break;
        case IA_Right:
            break;
        case IA_0:
            break;
        case IA_1:
        case IA_2:
        case IA_3:
        case IA_4:
        case IA_5:
        case IA_6:
        case IA_7:
        case IA_8:
        case IA_9:
        {
            int index = action - IA_1;
            m_CurrentlySelected = std::min(index, (int)m_Choices.size() - 1);
        }
        break;
        case IA_HOME:
            m_CurrentlySelected = 0;
            break;
        case IA_END:
            m_CurrentlySelected = static_cast<int>(m_Choices.size()) - 1;
            break;
        case IA_Close:
            // closing Dialog-Option-Box when pressing Escape
            //m_Engine.getMainWorld().get().getDialogManager().performChoice(m_Choices.size()-1);break;
            // selecting last option in Dialog-Option-Box when pressing Escape
            //m_CurrentlySelected = m_Choices.size()-1;break;
            break;
        case IA_Accept:
            if (m_CurrentlySelected != -1)
                m_Engine.getMainWorld().get().getDialogManager().performChoice(static_cast<size_t>(m_CurrentlySelected));
            break;
        default:
            break;
    }
}
