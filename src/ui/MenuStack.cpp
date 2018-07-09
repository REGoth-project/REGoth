//
// Created by markusobi on 05.06.18.
//

#include "MenuStack.h"
#include "Hud.h"
#include <engine/BaseEngine.h>

namespace UI
{

MenuStack::MenuStack(Engine::BaseEngine& e)
    : View(e)
{
}

bool MenuStack::isEmpty()
{
    return m_Children.empty();
}


void MenuStack::pop()
{
    // Move to other list to delete in the next frame. This makes it possible for menus to close themselves.
    m_MenusToDelete.push_back(m_Children.back());

    removeChild(m_Children.back());
    if (m_Children.empty())
    {
        m_Engine.getHud().setGameplayHudVisible(true);
        m_Engine.setPaused(false);
    }
}

void MenuStack::cleanMenus()
{
    for (View* menu : m_MenusToDelete)
        delete menu;

    m_MenusToDelete.clear();
}

void MenuStack::popAll()
{
    while (!m_Children.empty())
        pop();
}

MenuStack::~MenuStack()
{
    popAll();
    cleanMenus();
}

}