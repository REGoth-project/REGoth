//
// Created by desktop on 17.12.16.
//

#include "Hud.h"
#include "BarView.h"
#include "TextView.h"
#include <engine/BaseEngine.h>
#include "TextView.h"
#include "Menu_Status.h"
#include "Menu_Main.h"
#include "Menu_Load.h"
#include "Menu_Save.h"
#include "Menu_Settings.h"
#include "DialogBox.h"
#include <utils/logger.h>

UI::Hud::Hud(Engine::BaseEngine& e) : View(e)
{
    Textures::TextureAllocator& alloc = m_Engine.getEngineTextureAlloc();

    m_pHealthBar = new BarView(m_Engine);
    m_pManaBar = new BarView(m_Engine);
    m_pEnemyHealthBar = new BarView(m_Engine);
    m_pClock = new TextView(m_Engine);
    m_pDialogBox = new DialogBox(m_Engine);
    m_pDialogBox->setHidden(true);

    addChild(m_pHealthBar);
    addChild(m_pManaBar);
    addChild(m_pEnemyHealthBar);
    addChild(m_pClock);
    addChild(m_pDialogBox);

    // Initialize status bars
    {
        m_GameplayHudElements.push_back(m_pManaBar);
        //m_GameplayHudElements.push_back(m_pEnemyHealthBar);
        m_GameplayHudElements.push_back(m_pClock);
        m_GameplayHudElements.push_back(m_pHealthBar);

        Handle::TextureHandle hBarBackground = alloc.loadTextureVDF("BAR_BACK.TGA");
        Handle::TextureHandle hBarHealth = alloc.loadTextureVDF("BAR_HEALTH.TGA");
        Handle::TextureHandle hBarMana = alloc.loadTextureVDF("BAR_MANA.TGA");

        if(hBarMana.isValid() && hBarHealth.isValid() && hBarBackground.isValid())
        {
            // Images
            m_pHealthBar->setBackgroundImage(alloc.getTexture(hBarBackground));
            m_pManaBar->setBackgroundImage(alloc.getTexture(hBarBackground));
            m_pEnemyHealthBar->setBackgroundImage(alloc.getTexture(hBarBackground));

            m_pHealthBar->setBarImage(alloc.getTexture(hBarHealth));
            m_pManaBar->setBarImage(alloc.getTexture(hBarMana));
            m_pEnemyHealthBar->setBarImage(alloc.getTexture(hBarHealth));

            // Alignment
            m_pHealthBar->setAlignment(A_BottomLeft);
            m_pManaBar->setAlignment(A_BottomRight);
            m_pEnemyHealthBar->setAlignment(A_TopCenter);

            // Size
            Math::float2 barSize = Math::float2(0.6f, 0.6f);
            m_pHealthBar->setSize(barSize);
            m_pManaBar->setSize(barSize);
            m_pEnemyHealthBar->setSize(barSize);

            // Position
            m_pHealthBar->setTranslation(Math::float2(0.01f, 0.99f));
            m_pManaBar->setTranslation(Math::float2(0.99f, 0.99f));
            m_pEnemyHealthBar->setTranslation(Math::float2(0.5f, 0.01f));

            m_pEnemyHealthBar->setHidden(true);
        }
    }

    // Initialize clock
    {
        m_pClock->setTranslation(Math::float2(0.99f, 0.01f));
        m_pClock->setAlignment(A_TopRight);
    }
}

UI::Hud::~Hud()
{
    removeChild(m_pHealthBar);
    removeChild(m_pManaBar);
    removeChild(m_pEnemyHealthBar);
    removeChild(m_pClock);
    removeChild(m_pDialogBox);

    popAllMenus();

    delete m_pManaBar;
    delete m_pHealthBar;
    delete m_pEnemyHealthBar;
    delete m_pClock;
    delete m_pDialogBox;
}

void UI::Hud::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    // Free old menus
    cleanMenus();

    // Only draw last menu in the menu-chain
    if(!m_MenuChain.empty())
    {
        for(Menu* m : m_MenuChain)
            m->setHidden(true);

        m_MenuChain.back()->setHidden(false);
    }

    View::update(dt, mstate, config);
}

void UI::Hud::setHealth(float value)
{
    m_pHealthBar->setValue(value);
}

void UI::Hud::setMana(float value)
{
    m_pManaBar->setValue(value);
}

void UI::Hud::setEnemyHealth(float value)
{
    m_pEnemyHealthBar->setValue(value);
}

void UI::Hud::setDateTimeDisplay(const std::string &timeStr)
{
    m_pClock->setText(timeStr);
}

void UI::Hud::onTextInput(const std::string& text)
{
    if(m_Console.isOpen())
        m_Console.onTextInput(text);
    else if(!m_MenuChain.empty())
        m_MenuChain.back()->onTextInput(text);

}

void UI::Hud::onInputAction(UI::EInputAction action)
{
    auto& dialogManager = m_Engine.getMainWorld().get().getDialogManager();

    // Notify last menu in chain
    if(!m_MenuChain.empty() && action != IA_Close)
    {
        m_MenuChain.back()->onInputAction(action);
        return;
    }else if(dialogManager.isDialogActive())
    {
        if(!m_pDialogBox->isHidden()){
            m_pDialogBox->onInputAction(action);
            return;
        }
        else if (dialogManager.isTalking() && action == IA_Close) {
            dialogManager.cancelTalk();
            return;
        }
        return;
    }

    // Close console or last menu, in case it's open
    if(action == IA_Close)
    {
        if(m_Console.isOpen())
        {
            m_Console.setOpen(false);
            return;
        }
        else if(!m_MenuChain.empty())
        {
            popMenu();
            return;
        }
        else
        {
            // Nothing is open right now. Show main-menu
            pushMenu<UI::Menu_Main>();
            return;
        }
    }
}

void UI::Hud::setGameplayHudVisible(bool value)
{
    for(View* v : m_GameplayHudElements)
        v->setHidden(!value);
}

void UI::Hud::popMenu()
{
    // Move to other list to delete in the next frame. This makes it possible for menus to close themselves.
    m_MenusToDelete.push_back(m_MenuChain.back());

    removeChild(m_MenuChain.back());
    m_MenuChain.pop_back();
    if (m_MenuChain.empty())
    {
        m_Engine.setPaused(false);
    }
}

void UI::Hud::cleanMenus()
{
    for(Menu* m : m_MenusToDelete)
        delete m;

    m_MenusToDelete.clear();
}

void UI::Hud::popAllMenus() {
    while(!m_MenuChain.empty())
        popMenu();
}


