//
// Created by desktop on 17.12.16.
//

#include "Hud.h"
#include "BarView.h"
#include "DialogBox.h"
#include "LoadingScreen.h"
#include "Menu_Load.h"
#include "Menu_Log.h"
#include "Menu_Main.h"
#include "Menu_Save.h"
#include "Menu_Settings.h"
#include "Menu_Status.h"
#include "PrintScreenMessages.h"
#include "TextView.h"
#include "ItemGrid.h"
#include <components/VobClasses.h>
#include <engine/BaseEngine.h>
#include <logic/PlayerController.h>
#include <utils/logger.h>
#include <logic/ScriptEngine.h>

UI::Hud::Hud(Engine::BaseEngine& e)
    : View(e)
{
    Textures::TextureAllocator& alloc = m_Engine.getEngineTextureAlloc();

    m_pHealthBar = new BarView(m_Engine);
    m_pManaBar = new BarView(m_Engine);
    m_pEnemyHealthBar = new BarView(m_Engine);
    m_pDialogBox = new DialogBox(m_Engine);
    m_pDialogBox->setHidden(true);
    m_pPrintScreenMessageView = new PrintScreenMessages(m_Engine);
    m_pClock = new TextView(m_Engine);
    m_pLoadingScreen = new LoadingScreen(m_Engine);
    m_pLoadingScreen->setHidden(true);
    m_pConsoleBox = new ConsoleBox(m_Engine);
    m_pConsoleBox->setHidden(true);
    m_pMenuBackground = new ImageView(m_Engine);
    m_pMenuBackground->setHidden(true);
    m_pMenuBackground->setRelativeSize(false);
    m_pItemGrid = new ItemGrid(m_Engine);


    addChild(m_pHealthBar);
    addChild(m_pManaBar);
    addChild(m_pEnemyHealthBar);
    addChild(m_pDialogBox);
    addChild(m_pPrintScreenMessageView);
    addChild(m_pClock);
    addChild(m_pLoadingScreen);
    addChild(m_pMenuBackground);
    addChild(m_pConsoleBox);
    addChild(m_pItemGrid);

    // Initialize status bars
    {
        m_GameplayHudElements.push_back(m_pManaBar);
        //m_GameplayHudElements.push_back(m_pEnemyHealthBar);
        m_GameplayHudElements.push_back(m_pClock);
        m_GameplayHudElements.push_back(m_pHealthBar);

        // Background shown when there is no world loaded
        Handle::TextureHandle hBackground = alloc.loadTextureVDF("STARTSCREEN.TGA");
        m_pMenuBackground->setImage(hBackground);
        m_pMenuBackground->setSize(Math::float2(1, 1));

        Handle::TextureHandle hBarBackground = alloc.loadTextureVDF("BAR_BACK.TGA");
        Handle::TextureHandle hBarHealth = alloc.loadTextureVDF("BAR_HEALTH.TGA");
        Handle::TextureHandle hBarMana = alloc.loadTextureVDF("BAR_MANA.TGA");

        if (hBarMana.isValid() && hBarHealth.isValid() && hBarBackground.isValid())
        {
            // Images
            m_pHealthBar->setBackgroundImage(hBarBackground);
            m_pManaBar->setBackgroundImage(hBarBackground);
            m_pEnemyHealthBar->setBackgroundImage(hBarBackground);

            m_pHealthBar->setBarImage(hBarHealth);
            m_pManaBar->setBarImage(hBarMana);
            m_pEnemyHealthBar->setBarImage(hBarHealth);

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
    removeChild(m_pDialogBox);
    removeChild(m_pPrintScreenMessageView);
    removeChild(m_pClock);
    removeChild(m_pLoadingScreen);
    removeChild(m_pConsoleBox);
    removeChild(m_pMenuBackground);
    removeChild(m_pItemGrid);

    popAllMenus();

    delete m_pHealthBar;
    delete m_pManaBar;
    delete m_pEnemyHealthBar;
    delete m_pPrintScreenMessageView;
    delete m_pDialogBox;
    delete m_pClock;
    delete m_pLoadingScreen;
    delete m_pConsoleBox;
    delete m_pItemGrid;
}

void UI::Hud::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    // Free old menus
    cleanMenus();

    // Only draw last menu in the menu-chain
    if (!m_MenuChain.empty())
    {
        for (Menu* m : m_MenuChain)
            m->setHidden(true);

        m_MenuChain.back()->setHidden(false);
    }

    // Show the background, if there is no world loaded at the moment and loading isn't active
    const bool isWorldLoaded = m_Engine.getMainWorld().isValid();
    m_pMenuBackground->setHidden(isWorldLoaded || !m_pLoadingScreen->isHidden());

    View::update(dt, mstate, config);
}

void UI::Hud::setHealth(int32_t value, int32_t maxValue)
{
    m_pHealthBar->setValue(value, maxValue);
}

void UI::Hud::setMana(int32_t value, int32_t maxValue)
{
    m_pManaBar->setValue(value, maxValue);
}

void UI::Hud::setEnemyHealth(int32_t value, int32_t maxValue)
{
    m_pEnemyHealthBar->setValue(value, maxValue);
}

void UI::Hud::setDateTimeDisplay(const std::string& timeStr)
{
    m_pClock->setText(timeStr);
}

void UI::Hud::onTextInput(const std::string& text)
{
    if (m_Engine.getConsole().isOpen())
        m_Engine.getConsole().onTextInput(text);
    else if (!m_MenuChain.empty())
        m_MenuChain.back()->onTextInput(text);
}

void UI::Hud::onInputAction(UI::EInputAction action)
{
    if (!m_pLoadingScreen->isHidden())
        return;

    if (m_Engine.getConsole().isOpen())
    {
        if (action == IA_Close || action == IA_ToggleConsole)
            m_Engine.getConsole().setOpen(false);
        return;
    }
    else if (!m_MenuChain.empty())
    {
        // Notify last menu in chain
        bool close = m_MenuChain.back()->onInputAction(action);
        if (close)
            popMenu();
        return;
    }
    else if (m_Engine.getMainWorld().isValid() && m_Engine.getMainWorld().get().getDialogManager().isDialogActive())
    {
        m_Engine.getMainWorld().get().getDialogManager().onInputAction(action);
        return;
    }

    // case: Nothing is open right now.
    switch (action)
    {
        case IA_Close:
            // Show main-menu
            pushMenu<UI::Menu_Main>();
            return;
        case IA_ToggleConsole:
            m_Engine.getConsole().setOpen(true);
            return;
        case IA_ToggleStatusMenu:
        {
            UI::Menu_Status& statsScreen = pushMenu<UI::Menu_Status>();
            // TODO: Refactor move this into menu_status.create/new function?
            // Update the players status menu once
            auto& s = m_Engine.getMainWorld().get().getScriptEngine();
            VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_Engine.getMainWorld().get(), s.getPlayerEntity());
            player.playerController->updateStatusScreen(statsScreen);
            return;
        }
        case IA_ToggleLogMenu:
        {
            LogInfo() << "Open log";
            pushMenu<UI::Menu_Log>();
            return;
        }
        default:
            return;
    }
}

void UI::Hud::setGameplayHudVisible(bool value)
{
    for (View* v : m_GameplayHudElements)
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
        setGameplayHudVisible(true);
        m_Engine.setPaused(false);
    }
}

void UI::Hud::cleanMenus()
{
    for (Menu* m : m_MenusToDelete)
        delete m;

    m_MenusToDelete.clear();
}

void UI::Hud::popAllMenus()
{
    while (!m_MenuChain.empty())
        popMenu();
}

void UI::Hud::toggleInventory()
{
    m_pItemGrid->setHidden(!m_pItemGrid->isHidden());
}
