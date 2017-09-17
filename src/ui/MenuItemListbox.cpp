//
// Created by kapitaenzufall on 20.08.17.
//

#include "MenuItemListbox.h"
#include "Menu.h"
#include "MenuItem.h"
#include "zFont.h"
#include <engine/BaseEngine.h>
#include <utils/logger.h>

using namespace UI;
using namespace MenuItemTypes;

MenuItemTypes::MenuItemListbox::MenuItemListbox(Engine::BaseEngine& e, UI::Menu& baseMenu,
                                                const Daedalus::GameState::MenuItemHandle& scriptHandle)
    : MenuItem(e, baseMenu, scriptHandle)
{
    m_Selection = 0;
    m_Focus = false;
    setHidden(true);
}

void MenuItemTypes::MenuItemListbox::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if (m_IsHidden)
        return;

    MenuItem::update(dt, mstate, config);

    if (!m_ScriptHandle.isValid())
        return;

    Daedalus::GEngineClasses::C_Menu_Item& item = getItemScriptData();
    const UI::zFont* fnt = m_Engine.getFontCache().getFont(item.fontName);

    if (!fnt)
        return;

    // Get position of the text
    Math::float2 absTranslation = getAbsoluteTranslation();
    int px = (int)(absTranslation.x * config.state.viewWidth + 0.5f);
    int py = (int)(absTranslation.y * config.state.viewHeight + 0.5f);

    // Scroll if there are more then 20 topics
    unsigned int py_scroll = 0;
    if (m_Selection > 19)
        py_scroll = (m_Selection - 19) * fnt->getFontHeight();

    // Draw all topic names
    for (unsigned int k = 0; k < m_Entries.size(); k++)
    {
        // Calculate py of a specific topic k
        int py_calc = (py - py_scroll) + (k * fnt->getFontHeight());
        if (py_calc >= py && py_calc < (py + (20 * fnt->getFontHeight())))
        {
            // Highlight the selected topic
            if ((k == m_Selection) && m_Focus)
                m_Entries.at(k)->drawEntry(px, py_calc, A_TopLeft, config, item.fontName, true);
            else
                m_Entries.at(k)->drawEntry(px, py_calc, A_TopLeft, config, item.fontName, false);
        }
    }
}

bool MenuItemTypes::MenuItemListbox::hasTopics()
{
    if (m_Entries.size() > 0)
        return true;
    else
        return false;
}

void MenuItemTypes::MenuItemListbox::addTopic(std::string topic_name, const Daedalus::GameState::LogTopic& topic)
{
    m_Entries.push_back(new MenuItemListboxEntry(m_Engine, m_BaseMenu, m_ScriptHandle, topic_name, topic));
}

std::vector<std::string> MenuItemTypes::MenuItemListbox::getSelectedTopicEntries()
{
    return m_Entries[m_Selection]->getEntries();
}

void MenuItemTypes::MenuItemListbox::setFocus(bool focus)
{
    m_Focus = focus;
}

void MenuItemTypes::MenuItemListbox::selectPreviousItem()
{
    if (m_Selection > 0)
        m_Selection--;
}

void MenuItemTypes::MenuItemListbox::selectNextItem()
{
    if (m_Selection < m_Entries.size() - 1)
        m_Selection++;
}