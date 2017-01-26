#pragma once
#include "Console.h"
#include "Menu.h"
#include "View.h"

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
         * To be called when there was text input since the last frame
         * @param text Characters input since the last frame
         */
        void onTextInput(const std::string& text);

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

        /**
         * Appends a menu to the current menu-chain.
         * @tparam T Type of menu to append. Must have a static 'create' function!
         */
        template<typename T>
        T& pushMenu();

        /**
         * Pops the last menu from the chain and frees its memory.
         */
        void popMenu();
        
        template <typename T>
        bool isTopMenu()
        {
            return dynamic_cast<T*>(m_MenuChain.empty() ? nullptr : m_MenuChain.back()) != nullptr;
        }
        /**
         * @return Whether a menu is currently active
         */
        bool isMenuActive(){ return !m_MenuChain.empty(); }
        
    protected:

        /**
         * Deletes all menus stored in the m_MenusToDelete-list
         */
        void cleanMenus();

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
        Menu_Load* m_pMenuLoad;
        Menu_Save* m_pMenuSave;
        Menu_Settings* m_pMenuSettings;

        /**
         * Chain of opened menus. Only the last one will be rendered and processed
         */
        std::list<Menu*> m_MenuChain;
        std::list<Menu*> m_MenusToDelete; // Menus to be deleted next frame

        /**
         * All menus registered here
         */
        std::vector<Menu*> m_RegisteredMenus;
    };

    template <typename T>
    inline T& Hud::pushMenu()
    {
        if (!m_MenuChain.empty() && dynamic_cast<T*>(m_MenuChain.back()) != nullptr)
        {
            return *static_cast<T*>(m_MenuChain.back());
        }

        T *menu = T::create(m_Engine);
        m_MenuChain.push_back(menu);
        addChild(m_MenuChain.back());
        return *menu;
    }
}
