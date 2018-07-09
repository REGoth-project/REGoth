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

bool DialogBox::consumesAction(Engine::ActionType actionType, float intensity)
{
    if (isHidden())
        return false;
    using Engine::ActionType;

    switch (actionType)
    {
        case ActionType::UI_Up:
            m_CurrentlySelected = Utils::mod(m_CurrentlySelected - 1, (int)m_Choices.size());
            break;
        case ActionType::UI_Down:
            m_CurrentlySelected = Utils::mod(m_CurrentlySelected + 1, (int)m_Choices.size());
            break;
        case ActionType::UI_0:
            break;
        case ActionType::UI_1:
        case ActionType::UI_2:
        case ActionType::UI_3:
        case ActionType::UI_4:
        case ActionType::UI_5:
        case ActionType::UI_6:
        case ActionType::UI_7:
        case ActionType::UI_8:
        case ActionType::UI_9:
        {
            int index = static_cast<int>(actionType) - static_cast<int>(ActionType::UI_1);
            m_CurrentlySelected = std::min(index, (int)m_Choices.size() - 1);
        }
            break;
        case ActionType::UI_Top:
            m_CurrentlySelected = 0;
            break;
        case ActionType::UI_Bottom:
            m_CurrentlySelected = static_cast<int>(m_Choices.size()) - 1;
            break;
        case ActionType::UI_Confirm:
            if (m_CurrentlySelected != -1)
                m_Engine.getMainWorld().get().getDialogManager().performChoice(static_cast<size_t>(m_CurrentlySelected));
            break;
        default:
            break;
    }
    return View::consumesAction(actionType, intensity);
}


// TODO remove
// alternative way of specifying bindings
/*
 void DialogBox::setupBindings()
{

    using Engine::ActionType;

    registerActionHandler(ActionType::UI_Up, [this](float intensity){
        m_CurrentlySelected = Utils::mod(m_CurrentlySelected - 1, (int)m_Choices.size());
    });
    registerActionHandler(ActionType::UI_Down, [this](float intensity){
        m_CurrentlySelected = Utils::mod(m_CurrentlySelected + 1, (int)m_Choices.size());
    });
    registerActionHandler(ActionType::UI_Top, [this](float intensity){
        m_CurrentlySelected = 0;
    });
    registerActionHandler(ActionType::UI_Bottom, [this](float intensity){
        m_CurrentlySelected = static_cast<int>(m_Choices.size()) - 1;
    });
    registerActionHandler(ActionType::UI_Confirm, [this](float intensity){
        if (m_CurrentlySelected != -1)
            m_Engine.getMainWorld().get().getDialogManager().performChoice(static_cast<size_t>(m_CurrentlySelected));
    });

    auto begin = ActionType::UI_1;
    auto last = ActionType::UI_9;
    for (auto it = begin; it <= last; it = static_cast<ActionType>(static_cast<int>(it) + 1))
    {
        registerActionHandler(ActionType::UI_Confirm, [this, it](float intensity){
            int index = static_cast<int>(it) - static_cast<int>(ActionType::UI_1);
            m_CurrentlySelected = std::min(index, (int)m_Choices.size() - 1);
        });
    }
}
 */