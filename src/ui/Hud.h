#pragma once
#include "Menu.h"
#include "View.h"
#include <engine/BaseEngine.h>
#include <engine/Input.h>
#include <logic/Console.h>

// HACK: Work around windows.h messing this up with its define
#ifdef DialogBox
#undef DialogBox
#endif

namespace Engine
{
    class BaseEngine;
}

namespace UI
{
    class BarView;
    class TextView;
    class Menu_Status;
    class Menu_Main;
    class Menu_Load;
    class Menu_Save;
    class Menu_Settings;
    class Menu;
    class DialogBox;
    class LoadingScreen;
    class IntroduceChapterView;
    class WorldView;
    class MenuStack;

    class Hud : public View
    {
    public:
        Hud(Engine::BaseEngine& e);
        ~Hud();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        /**
         * @param value for health-bar of current player
         * @param maxValue for health-bar of current player
         */
        void setHealth(int32_t value, int32_t maxValue);

        /**
         * @param value for mana-bar of current player
         * @param maxValue for mana-bar of current player
         */
        void setMana(int value, int maxValue);

        /**
         * @param value for health-bar of enemy entity
         * @param maxValue for health-bar of enemy entity
         */
        void setEnemyHealth(int32_t value, int32_t maxValue);

        /**
         * @param timeStr Current time of day to be shown on the hud
         */
        void setDateTimeDisplay(const std::string& timeStr);

        /**
         * @return WorldView
         */
        WorldView& getWorldView() { return *m_pWorldView; }

        /**
         * @return MenuStack
         */
        MenuStack& getMenuStack() { return *m_pMenuStack; }

        /**
         * @return Console-Box
         */
        ConsoleBox& getConsoleBox() { return *m_pConsoleBox; }
        /**
         * @return Dialog-box
         */
        DialogBox& getDialogBox() { return *m_pDialogBox; }
        /**
         * @return PrintScreenManager
         */
        PrintScreenMessages& getPrintScreenManager() { return *m_pPrintScreenMessageView; }
        /**
         * LoadingScreen
         */
        LoadingScreen& getLoadingScreen() { return *m_pLoadingScreen; }
        /**
         * IntroduceChapterView
         */
        IntroduceChapterView& getIntroduceChapterView() { return *m_pIntroduceChapterView; }
        /**
         * Controls visibility of gameplay-hud
         */
        void setGameplayHudVisible(bool value);

        /**
         * @return Whether a menu is currently active
         */
        bool isMenuActive();

    protected:

        bool consumesAction(Engine::ActionType actionType, float intensity) override;

        /**
         * All views qualifying as used while normal gameplay
         */
        std::vector<View*> m_GameplayHudElements;

        BarView* m_pHealthBar;
        BarView* m_pManaBar;
        BarView* m_pEnemyHealthBar;
        TextView* m_pClock;
        DialogBox* m_pDialogBox;
        PrintScreenMessages* m_pPrintScreenMessageView;
        LoadingScreen* m_pLoadingScreen;
        ImageView* m_pMenuBackground;
        ConsoleBox* m_pConsoleBox;
        IntroduceChapterView* m_pIntroduceChapterView;
        WorldView* m_pWorldView;
        MenuStack* m_pMenuStack;
    };
}

