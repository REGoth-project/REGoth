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
#include "MenuStack.h"
#include "PrintScreenMessages.h"
#include "TextView.h"
#include "IntroduceChapterView.h"
#include "WorldView.h"
#include <components/VobClasses.h>
#include <logic/PlayerController.h>
#include <logic/CameraController.h>
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
    m_pMenuStack = new MenuStack(m_Engine);
    m_pPrintScreenMessageView = new PrintScreenMessages(m_Engine);
    m_pClock = new TextView(m_Engine);
    m_pLoadingScreen = new LoadingScreen(m_Engine);
    m_pLoadingScreen->setHidden(true);
    m_pConsoleBox = new ConsoleBox(m_Engine);
    m_pConsoleBox->setHidden(true);
    m_pMenuBackground = new ImageView(m_Engine);
    m_pMenuBackground->setHidden(true);
    m_pMenuBackground->setRelativeSize(false);
    m_pIntroduceChapterView = new IntroduceChapterView(m_Engine);
    m_pIntroduceChapterView->setHidden(true);
    m_pWorldView = new WorldView(m_Engine);

    addChild(m_pWorldView);
    addChild(m_pHealthBar);
    addChild(m_pManaBar);
    addChild(m_pEnemyHealthBar);
    addChild(m_pDialogBox);
    addChild(m_pMenuStack);
    addChild(m_pPrintScreenMessageView);
    addChild(m_pClock);
    addChild(m_pLoadingScreen);
    addChild(m_pMenuBackground);
    addChild(m_pConsoleBox);
    addChild(m_pIntroduceChapterView);

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
    removeChild(m_pWorldView);
    removeChild(m_pHealthBar);
    removeChild(m_pManaBar);
    removeChild(m_pEnemyHealthBar);
    removeChild(m_pDialogBox);
    removeChild(m_pMenuStack);
    removeChild(m_pPrintScreenMessageView);
    removeChild(m_pClock);
    removeChild(m_pLoadingScreen);
    removeChild(m_pConsoleBox);
    removeChild(m_pMenuBackground);
    removeChild(m_pIntroduceChapterView);

    delete m_pWorldView;
    delete m_pHealthBar;
    delete m_pManaBar;
    delete m_pEnemyHealthBar;
    delete m_pPrintScreenMessageView;
    delete m_pDialogBox;
    delete m_pMenuStack;
    delete m_pClock;
    delete m_pLoadingScreen;
    delete m_pConsoleBox;
    delete m_pIntroduceChapterView;
}

void UI::Hud::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    // Show the background, if there is no world loaded at the moment and loading isn't active
    const bool isWorldLoaded = m_Engine.getMainWorld().isValid();
    m_pMenuBackground->setHidden(isWorldLoaded || !m_pLoadingScreen->isHidden());

    // Only show gameplay hud when menu stack is empty
    m_Engine.getHud().setGameplayHudVisible(m_pMenuStack->isEmpty());

    View::update(dt, mstate, config);

    // TODO Camera should not be enabled/disabled every frame.
    if (isWorldLoaded) {
        // Deactivate camera controls when menu or dialog is open
        bool disable = !(isMenuActive()
                         || m_Engine.getMainWorld().get().getDialogManager().isDialogActive()
                         || m_Engine.getConsole().isOpen());
        m_Engine.getMainWorld().get().getCameraController()->setActive(disable);
    }
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

// TODO bind MWheel Up/Down to UI_Up/UI_Down
/*
auto mousewheelBinding = registerAction(ActionType::UI_Mousewheel, [this](bool triggered, float intensity){
    if (triggered) {
        if (intensity == 1.f)
            onInputAction(ActionType::UI_Up, triggered, 1.0f);
        else if (intensity == -1.f)
            onInputAction(ActionType::UI_Down, triggered, 1.0f);
    }
});
 */

bool UI::Hud::consumesAction(Engine::ActionType actionType, float intensity)
{
    if (isHidden())
        return false;

    using Engine::ActionType;
    // camera change actions
    using ECameraMode = Logic::CameraController::ECameraMode;
    static const std::map<Engine::ActionType, ECameraMode> cameraModes = {
        {ActionType::CameraFirstPerson, ECameraMode::FirstPerson},
        {ActionType::CameraThirdPerson, ECameraMode::ThirdPerson},
        {ActionType::CameraFree, ECameraMode::Free},
        {ActionType::CameraViewer, ECameraMode::Viewer},
    };

    // TODO make getMainWorld().isValid() checks redundant?
    switch (actionType)
    {
        case ActionType::UI_Mousewheel: // TODO remove mouse wheel action?
            if (intensity == 1.f)
                return consumesAction(ActionType::UI_Up, 1.0f);
            else if (intensity == -1.f)
                return consumesAction(ActionType::UI_Down, 1.0f);
            break;
        case ActionType::Quicksave:
            // better do saving at frame end and not between entity updates
            m_Engine.getJobManager().queueJob([](Engine::BaseEngine* engine){
                Engine::SavegameManager::saveToSlot(0, "");
            });
            break;
        case ActionType::Quickload:
            Engine::SavegameManager::loadSaveGameSlot(0);
            break;
        case ActionType::PauseGame:
            m_Engine.togglePaused();
            break;
        case ActionType::UI_Close:
        case ActionType::UI_ToggleMainMenu:
            // Show main-menu
            m_pMenuStack->push<UI::Menu_Main>();
            break;
        case ActionType::UI_ToggleStatusMenu:
        {
            auto& statsScreen = m_pMenuStack->push<UI::Menu_Status>();
            // TODO: Refactor move this into menu_status.create/new function?
            // Update the players status menu once
            auto& s = m_Engine.getMainWorld().get().getScriptEngine();
            VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_Engine.getMainWorld().get(), s.getPlayerEntity());
            player.playerController->updateStatusScreen(statsScreen);
            break;
        }
        case ActionType::UI_ToggleConsole:
            m_Engine.getConsole().setOpen(true);
            break;
        case ActionType::CameraFirstPerson:
        case ActionType::CameraThirdPerson:
        case ActionType::CameraFree:
        case ActionType::CameraViewer:
            if (m_Engine.getMainWorld().isValid())
                m_Engine.getMainWorld().get().getCameraController()->setCameraMode(cameraModes.at(actionType));
            break;
        case ActionType::DebugMoveSpeed:
        case ActionType::DebugMoveSpeed2:
            if (m_Engine.getMainWorld().isValid())
                throw std::runtime_error("unimplemented")// TODO how to handle contiguous actions?
                ;//m_Engine.getMainWorld().get().getCameraController()->setDebugMoveSpeed(?);
            // TODO speed up Player (former handled in Playercontroller::onAction)
            break;
        case ActionType::UI_ToggleLogMenu:
            m_pMenuStack->push<UI::Menu_Log>();
            break;
        case ActionType::PlayerDrawWeaponMelee:
        case ActionType::PlayerForward:
        case ActionType::PlayerBackward:
        case ActionType::PlayerTurnLeft:
        case ActionType::PlayerTurnRight:
        case ActionType::PlayerStrafeLeft:
        case ActionType::PlayerStrafeRight:
        case ActionType::PlayerAction:
        case ActionType::PlayerActionContinous:
        case ActionType::PlayerRotate:
        {
            auto worldHandle = m_Engine.getMainWorld();
            if (!worldHandle.isValid())
                break;

            auto player = worldHandle.get().getScriptEngine().getPlayerEntity();
            if (!player.isValid())
                break;

            auto playervob = VobTypes::asNpcVob(worldHandle.get(), player);
            if (playervob.isValid())
                playervob.playerController->onAction(actionType, intensity); //
        }
            break;
        default:
            break;
    }
    // TODO pop menu when it returns false or pop menu self?
    return View::onInputAction(actionType, intensity);
}

void UI::Hud::setGameplayHudVisible(bool value)
{
    for (View* v : m_GameplayHudElements)
        v->setHidden(!value);
}

bool UI::Hud::isMenuActive()
{
    return !m_pMenuStack->isEmpty();
}

