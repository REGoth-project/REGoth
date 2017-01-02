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

    // Menus
    m_pStatusMenu = Menu_Status::create(m_Engine);
    m_RegisteredMenus.push_back(m_pStatusMenu);
    m_pStatusMenu->setHidden(true);

    m_pMainMenu = Menu_Main::create(m_Engine);
    m_RegisteredMenus.push_back(m_pMainMenu);
    m_pMainMenu->setHidden(true);

    m_pMenuLoad = Menu_Load::create(m_Engine);
    m_RegisteredMenus.push_back(m_pMenuLoad );
    m_pMenuLoad->setHidden(true);

    m_pMenuSave = Menu_Save::create(m_Engine);
    m_RegisteredMenus.push_back(m_pMenuSave);
    m_pMenuSave->setHidden(false);

    m_pMenuSettings = Menu_Settings::create(m_Engine);
    m_RegisteredMenus.push_back(m_pMenuSettings);
    m_pMenuSettings->setHidden(true);
    
    for(Menu* m : m_RegisteredMenus)
        addChild(m);

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

    for(Menu* m : m_RegisteredMenus)
    {
        removeChild(m);
        delete m;
    }

    delete m_pManaBar;
    delete m_pHealthBar;
    delete m_pEnemyHealthBar;
    delete m_pClock;
    delete m_pDialogBox;
}

void UI::Hud::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
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

void UI::Hud::setTimeOfDay(const std::string& timeStr)
{
    m_pClock->setText(timeStr);
}

void UI::Hud::onInputAction(UI::EInputAction action)
{
    // Close console, in case it's open
    if(action == IA_Close)
    {
        m_Console.setOpen(false);

        // FIXME: Menu-bindings need to be reworked!
        if(m_pDialogBox->isHidden() && m_pStatusMenu->isHidden() )
        {
            m_pMainMenu->setHidden(!m_pMainMenu->isHidden());
        }
    }
    // Notify all menus
    // TODO: Do this in a loop
    if(!m_pStatusMenu->isHidden())
        m_pStatusMenu->onInputAction(action);

    if(!m_pMenuLoad->isHidden())
        m_pMenuLoad->onInputAction(action);

    if(!m_pMenuSave->isHidden())
        m_pMenuSave->onInputAction(action);

    if(!m_pMenuSettings->isHidden())
        m_pMenuSettings->onInputAction(action);
    
    if(!m_pDialogBox->isHidden())
        m_pDialogBox->onInputAction(action);

    if(!m_pMainMenu->isHidden() && action != IA_Close)
        m_pMainMenu->onInputAction(action);
}

void UI::Hud::setGameplayHudVisible(bool value)
{
    for(View* v : m_GameplayHudElements)
        v->setHidden(!value);
}
