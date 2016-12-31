#pragma once
#include "View.h"
#include "Console.h"

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
    class DialogBox;
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
        void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config) override;

        /**
         * @param value Value for health-bar in 0..1 range
         */
        void setHealth(float value);

        /**
         * @param value Value for mana-bar in 0..1 range
         */
        void setMana(float value);

        /**
         * @param value Value for enemy health-bar in 0..1 range
         */
        void setEnemyHealth(float value);

        /**
         * @param timeStr Current time of day to be shown on the hud
         */
        void setTimeOfDay(const std::string& timeStr);

        /**
         * To be called when one of the given actions were triggered
         * @param action Input action
         */
        void onInputAction(EInputAction action);

        /**
         * @return Access to the status-menu
         */
        Menu_Status& getStatusMenu(){ return *m_pStatusMenu; }

        /**
         * @return Games console
         */
        UI::Console& getConsole(){ return m_Console; }

        /**
         * @return Dialog-box
         */
        DialogBox& getDialogBox(){ return *m_pDialogBox; }

        /**
         * Controls visibility of gameplay-hud
         */
         void setGameplayHudVisible(bool value);
    protected:

        /**
         * All views qualifying as used while normal gameplay
         */
        std::vector<View*> m_GameplayHudElements;

        BarView* m_pHealthBar;
        BarView* m_pManaBar;
        BarView* m_pEnemyHealthBar;
        TextView* m_pClock;
        DialogBox* m_pDialogBox;

        /**
         * Console
         */
        UI::Console m_Console;

        /**
         * Menus
         */
        Menu_Status* m_pStatusMenu;
        Menu_Main* m_pMainMenu;
    };
}
