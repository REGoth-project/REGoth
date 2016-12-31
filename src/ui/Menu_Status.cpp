//
// Created by desktop on 21.12.16.
//

#include "Menu_Status.h"
#include <engine/BaseEngine.h>

using namespace UI;



Menu_Status::Menu_Status(Engine::BaseEngine& e) : Menu(e)
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
    if(!m_MenuHandle.isValid())
        return;

    bool hasMax = false;
    std::string name = "MENU_ITEM_ATTRIBUTE_" + std::to_string(type);
    switch(type)
    {
        case A_STR: break;
        case A_DEX: break;
        case A_MANA: hasMax = true; break;
        case A_HEALTH: hasMax = true; break;
    }

    getItemScriptData(name).text[0] = std::to_string(value) + (hasMax ? "/" + std::to_string(max) : "");
}

void Menu_Status::setGuild(const std::string& guild)
{
    if(!m_MenuHandle.isValid())
        return;
    getItemScriptData("MENU_ITEM_PLAYERGUILD").text[0] = guild;
}

void Menu_Status::setLevel(int level)
{
    if(!m_MenuHandle.isValid())
        return;

    getItemScriptData("MENU_ITEM_LEVEL").text[0] = std::to_string(level);
}

void Menu_Status::setExperience(int xp)
{
    if(!m_MenuHandle.isValid())
        return;

    getItemScriptData("MENU_ITEM_EXP").text[0] = std::to_string(xp);
}

void Menu_Status::setLearnPoints(int lp)
{
    if(!m_MenuHandle.isValid())
        return;

    getItemScriptData("MENU_ITEM_LEARN").text[0] = std::to_string(lp);
}

void Menu_Status::setExperienceNext(int xpNext)
{
    if(!m_MenuHandle.isValid())
        return;

    getItemScriptData("MENU_ITEM_LEVEL_NEXT").text[0] = std::to_string(xpNext);
}

void Menu_Status::onInputAction(EInputAction action)
{
    Menu::onInputAction(action);

    switch(action)
    {
        case IA_Up:break;
        case IA_Down:break;
        case IA_Left:break;
        case IA_Right:break;
        case IA_Close: break;
        case IA_Accept:break;
    }
}
