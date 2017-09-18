//
// Created by desktop on 21.12.16.
//

#include "MenuItem.h"
#include "ImageView.h"
#include "Menu.h"
#include "zFont.h"
#include <engine/GameEngine.h>
#include <utils/logger.h>

using namespace UI;

MenuItem::MenuItem(Engine::GameEngine& e, Menu& baseMenu, Daedalus::GameState::MenuItemHandle scriptHandle)
    : View(e)
    , m_ScriptHandle(scriptHandle)
    , m_BaseMenu(baseMenu)
{
    // Set position and other values according to script-data
    Daedalus::GEngineClasses::C_Menu_Item& item = getItemScriptData();

    setTranslation(Math::float2(item.posx / 8192.0f, item.posy / 8192.0f));

    size_t dimx = 8192;
    size_t dimy = 750;

    if (item.dimx != -1) dimx = item.dimx;
    if (item.dimy != -1) dimy = item.dimy;

    setSize(Math::float2(dimx / 8192.0f, dimy / 8192.0f));

    if ((item.flags & Daedalus::GEngineClasses::C_Menu_Item::IT_TXT_CENTER) != 0)
    {
        //setTranslation(m_Translation * 0.5f + (m_Translation + m_Size) * 0.5f);
        //setAlignment(A_Center);
    }

    m_pBackgroundImage = nullptr;
    if (!item.backPic.empty())
    {
        m_pBackgroundImage = new ImageView(m_Engine);
        // m_pBackgroundImage->setTranslation(Math::float2(item.posx / 8192.0f, item.posy / 8192.0f));
        // m_pBackgroundImage->setSize(Math::float2(item.dimx / 8192.0f, item.dimy / 8192.0f));

        Handle::TextureHandle bgr = m_Engine.getEngineTextureAlloc().loadTextureVDF(getItemScriptData().backPic);
        m_pBackgroundImage->setImage(bgr);
        m_pBackgroundImage->setRelativeSize(false);  // Scale like this screen
        m_pBackgroundImage->setAlignment(A_Center);
        addChild(m_pBackgroundImage);
    }
}

MenuItem::~MenuItem()
{
    removeChild(m_pBackgroundImage);
    delete m_pBackgroundImage;
}

void MenuItem::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if (m_IsHidden)
        return;

    if (m_pBackgroundImage)  // Background image shouldn't be centered in any way
        m_pBackgroundImage->setAlignment(A_TopLeft);

    View::update(dt, mstate, config);

    if (!m_ScriptHandle.isValid())
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

MenuItemTypes::MenuItemText::MenuItemText(Engine::GameEngine& e, UI::Menu& baseMenu,
                                          const Daedalus::GameState::MenuItemHandle& scriptHandle)
    : MenuItem(e, baseMenu, scriptHandle)
{
}

void MenuItemTypes::MenuItemText::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
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
    Math::float2 absSize = getAbsoluteSize();
    int px = (int)(absTranslation.x * config.state.viewWidth + 0.5f);
    int py = (int)(absTranslation.y * config.state.viewHeight + 0.5f);

    if (!item.text[0].empty())
    {
        // Alignment must be taken care of for text only, as it would mess up the background-pic otherwise
        if ((item.flags & Daedalus::GEngineClasses::C_Menu_Item::IT_TXT_CENTER) != 0)
        {
            int sx = (int)(absSize.x * config.state.viewWidth + 0.5f);
            int sy = (int)(absSize.y * config.state.viewHeight + 0.5f);

            px += sx / 2;
            py += sy / 2;
        }

        EAlign align = (item.flags & Daedalus::GEngineClasses::C_Menu_Item::IT_TXT_CENTER) != 0 ? A_Center : A_TopLeft;
        drawText(item.text[0], px, py, align, config, item.fontName);
    }
}

void MenuItem::setFontHighlighted(bool value)
{
    std::string fontBase = getItemScriptData().fontName;

    fontBase = Utils::stripExtension(fontBase);

    // Strip the _HI, if present
    if (fontBase.substr(fontBase.size() - 3) == "_HI")
        fontBase = fontBase.substr(0, fontBase.size() - 3);

    if (value)
        fontBase += "_HI";

    getItemScriptData().fontName = fontBase;
}

Daedalus::GEngineClasses::MenuConstants::ESelEvent MenuItem::getSelectionEvent(int i)
{
    assert(i < Daedalus::GEngineClasses::MenuConstants::MAX_SEL_ACTIONS);
    return (Daedalus::GEngineClasses::MenuConstants::ESelEvent)getItemScriptData().onSelAction[i];
}
