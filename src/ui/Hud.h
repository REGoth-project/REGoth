#pragma once
#include "Menu.h"
#include "View.h"
#include "VideoView.h"
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
         * Registers all key bindings for the HUD
         */
        void setupKeyBindings();

        /**
         * Un-registers all key bindings for the HUD
         */
        void clearKeyBindings();

        /**
         * To be called when one of the given actions were triggered
         * @param action Input action
         */
        void onInputAction(Engine::ActionType action);

        /**
         * To be called when there was text input since the last frame
         * @param text Characters input since the last frame
         */
        void onTextInput(const std::string& text);

        /**
         * @return Console-Box
         */
        UI::ConsoleBox& getConsoleBox() { return *m_pConsoleBox; }
        /**
         * @return Dialog-box
         */
        DialogBox& getDialogBox() { return *m_pDialogBox; }
        /**
         * @return PrintScreenManager
         */
        UI::PrintScreenMessages& getPrintScreenManager() { return *m_pPrintScreenMessageView; }
        /**
         * LoadingScreen
         */
        LoadingScreen& getLoadingScreen() { return *m_pLoadingScreen; }
        /**
         * Controls visibility of gameplay-hud
         */
        void setGameplayHudVisible(bool value);

        /**
         * Plays a fullscreen-video.
         * @param video Videofile to play
         * @param onPlaybackFinished Function to call when the video is done playing
         */
        void playVideo(const std::string& video);

        /**
         * Appends a menu to the current menu-chain.
         * @tparam T Type of menu to append. Must have a static 'create' function!
         */
        template <typename T>
        T& pushMenu();

        /**
         * Pops the last menu from the chain and frees its memory.
         */
        void popMenu();

        /**
         * Pops all menus from the chain and frees its memory.
         */
        void popAllMenus();

        template <typename T>
        bool isTopMenu()
        {
            return dynamic_cast<T*>(m_MenuChain.empty() ? nullptr : m_MenuChain.back()) != nullptr;
        }
        /**
         * @return Whether a menu is currently active
         */
        bool isMenuActive() { return !m_MenuChain.empty(); }

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
        PrintScreenMessages* m_pPrintScreenMessageView;
        LoadingScreen* m_pLoadingScreen;
        ImageView* m_pMenuBackground;
        ConsoleBox* m_pConsoleBox;
        VideoView* m_VideoView;

        /**
         * Chain of opened menus. Only the last one will be rendered and processed
         */
        std::list<Menu*> m_MenuChain;
        std::list<Menu*> m_MenusToDelete;  // Menus to be deleted next frame

        /**
         * All menus registered here
         */
        std::vector<Menu*> m_RegisteredMenus;

        /**
         * stored hud key bindings
         */
        std::vector<Engine::ManagedActionBinding> m_HudBindings;
    };

    template <typename T>
    inline T& Hud::pushMenu()
    {
        if (!m_MenuChain.empty() && dynamic_cast<T*>(m_MenuChain.back()) != nullptr)
        {
            return *static_cast<T*>(m_MenuChain.back());
        }

        // Hide gamplay hud since there is now at least one menu active
        setGameplayHudVisible(false);

        if (m_MenuChain.empty())
        {
            m_Engine.setPaused(true);
        }
        T* menu = T::create(m_Engine);
        m_MenuChain.push_back(menu);
        addChild(m_MenuChain.back());
        return *menu;
    }
}
