//
// Created by desktop on 21.12.16.
//

#include "Menu_Status.h"
#include <engine/BaseEngine.h>

using namespace UI;

Menu_Status::Menu_Status(Engine::BaseEngine& e)
    : Menu(e)
{
}

Menu_Status* Menu_Status::create(Engine::BaseEngine& e)
{
    Menu_Status* s = new Menu_Status(e);
    s->initializeInstance("MENU_STATUS");

    return s;
}

void Menu_Status::setAttribute(Menu_Status::EAttribute type, int value, int max)
{
    if (!m_MenuHandle.isValid())
        return;

    bool hasMax = false;
    std::string name = "MENU_ITEM_ATTRIBUTE_" + std::to_string(type);
    switch (type)
    {
        case A_STR:
            break;
        case A_DEX:
            break;
        case A_MANA:
            hasMax = true;
            break;
        case A_HEALTH:
            hasMax = true;
            break;
    }

    getItemScriptData(name).text[0] = std::to_string(value) + (hasMax ? "/" + std::to_string(max) : "");
}

void Menu_Status::setGuild(const std::string& guild)
{
    if (!m_MenuHandle.isValid())
        return;
    getItemScriptData("MENU_ITEM_PLAYERGUILD").text[0] = guild;
}

void Menu_Status::setLevel(int level)
{
    if (!m_MenuHandle.isValid())
        return;

    getItemScriptData("MENU_ITEM_LEVEL").text[0] = std::to_string(level);
}

void Menu_Status::setExperience(int xp)
{
    if (!m_MenuHandle.isValid())
        return;

    getItemScriptData("MENU_ITEM_EXP").text[0] = std::to_string(xp);
}

void Menu_Status::setLearnPoints(int lp)
{
    if (!m_MenuHandle.isValid())
        return;

    getItemScriptData("MENU_ITEM_LEARN").text[0] = std::to_string(lp);
}

void Menu_Status::setExperienceNext(int xpNext)
{
    if (!m_MenuHandle.isValid())
        return;

    getItemScriptData("MENU_ITEM_LEVEL_NEXT").text[0] = std::to_string(xpNext);
}

bool Menu_Status::onInputAction(Engine::ActionType action)
{
    bool baseclassClose = Menu::onInputAction(action);
    // close this menu if either the parent function wants to close or this function
    return baseclassClose || (action == Engine::ActionType::UI_ToggleStatusMenu);
}
