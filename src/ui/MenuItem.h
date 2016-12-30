//
// Created by desktop on 21.12.16.
//

#pragma once

#include <ZenLib/daedalus/DaedalusGameState.h>
#include "View.h"

namespace UI
{
    class Menu;
    class MenuItem : public View
    {
    public:
        MenuItem(Engine::BaseEngine& e, Menu& baseMenu, Daedalus::GameState::ItemHandle scriptHandle);

        void setTextLine(const std::string& text, unsigned int line = 0);

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config) override;

        /**
         * @return Script-data of this item
         */
        Daedalus::GEngineClasses::C_Menu_Item& getItemScriptData();

    protected:
        /**
         * Reference to the menu owning this
         */
        Menu& m_BaseMenu;

        /**
         * Handle to this items script-data
         */
        Daedalus::GameState::ItemHandle m_ScriptHandle;

        /**
         * Lines of text this view can render
         */
        std::vector<std::string> m_TextLines;
    };

    namespace MenuItemTypes
    {
        class MenuItemText : public MenuItem
        {
        public:
            MenuItemText(Engine::BaseEngine& e, Menu& baseMenu, const Daedalus::GameState::ItemHandle& scriptHandle);

            /**
             * Updates/draws the UI-Views
             * @param dt time since last frame
             * @param mstate mouse-state
             */
            void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config) override;
        };
    }
}
