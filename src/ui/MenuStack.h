//
// Created by markusobi on 05.06.18.
//
#pragma once
#include "View.h"
#include "Menu.h"
#include <engine/BaseEngine.h>

namespace UI
{
class MenuStack : public View
{
public:
    MenuStack(Engine::BaseEngine& e);
    ~MenuStack();

    bool isEmpty();

    /**
     * Appends a menu to the current menu-chain.
     */
    template <class Menu>
    Menu& push();

    /**
     * Pops the last menu from the chain and frees its memory.
     */
    void pop();

    /**
     * Pops all menus from the chain and frees its memory.
     */
    void popAll();

    /**
     * Deletes all menus stored in the m_MenusToDelete-list
     */
    void cleanMenus();

private:

    // Menus to be deleted next frame
    std::list<View*> m_MenusToDelete;
};

template <typename Menu>
inline Menu& MenuStack::push()
{
    if (m_Children.empty())
    {
        // TODO is there a better way to handle this to avoid BaseEngine include?
        m_Engine.setPaused(true);
    }
    Menu* menu = Menu::create(m_Engine);
    addChild(menu);
    return *menu;
}

}