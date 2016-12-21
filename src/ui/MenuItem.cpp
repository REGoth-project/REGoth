//
// Created by desktop on 21.12.16.
//

#include "MenuItem.h"
#include "Menu.h"
#include "zFont.h"
#include <engine/BaseEngine.h>

using namespace UI;

MenuItem::MenuItem(Engine::BaseEngine& e, Menu& baseMenu, Daedalus::GameState::ItemHandle scriptHandle)
        : View(e), m_ScriptHandle(scriptHandle), m_BaseMenu(baseMenu)
{
    // Set position and other values according to script-data
    Daedalus::GEngineClasses::C_Menu_Item& item = getItemScriptData();

    setTranslation(Math::float2(item.posx / 8192.0f, item.posy / 8192.0f));

    if(item.dimx != -1 && item.dimy != -1) // -1 = Default size
        setSize(Math::float2(item.dimx / 8192.0f, item.dimy / 8192.0f));

}

void MenuItem::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    View::update(dt, mstate, config);

    if(!m_ScriptHandle.isValid())
        return;

    Daedalus::GEngineClasses::C_Menu_Item& item = getItemScriptData();


}

Daedalus::GEngineClasses::C_Menu_Item& MenuItem::getItemScriptData()
{
    return m_BaseMenu.getItemScriptData(m_ScriptHandle);
}

void MenuItem::setTextLine(const std::string& text, unsigned int line)
{
    Daedalus::GEngineClasses::C_Menu_Item& item = getItemScriptData();

    item.text[line] = text;
}

/**
 *
 *
 *  ========================= MenuItemText =========================
 *
 *
 */

MenuItemTypes::MenuItemText::MenuItemText(  Engine::BaseEngine& e, UI::Menu& baseMenu,
                                            const Daedalus::GameState::ItemHandle& scriptHandle)
                                            : MenuItem(e, baseMenu, scriptHandle)
{

}

void MenuItemTypes::MenuItemText::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{


    MenuItem::update(dt, mstate, config);

    if(!m_ScriptHandle.isValid() )
        return;

    Daedalus::GEngineClasses::C_Menu_Item& item = getItemScriptData();
    const UI::zFont* fnt = m_Engine.getFontCache().getFont(item.fontName);

    if(!fnt)
        return;

    // Get position of the text
    Math::float2 absTranslation = getAbsoluteTranslation();
    Math::float2 absSize = getAbsoluteSize();
    int px = (int) (absTranslation.x * config.state.viewWidth + 0.5f);
    int py = (int) (absTranslation.y * config.state.viewHeight + 0.5f);

    for(unsigned int i=0;i<Daedalus::GEngineClasses::MenuConstants::MAX_USERSTRINGS; i++)
    {
        if(!item.text[i].empty())
            drawText(item.text[i], px, py + fnt->getFontHeight() * i, A_TopLeft, config, item.fontName);
    }
}
