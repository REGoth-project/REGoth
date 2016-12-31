//
// Created by desktop on 21.12.16.
//

#pragma once

#include <ZenLib/daedalus/DaedalusGameState.h>
#include "View.h"

namespace UI
{
    class Menu;
    class ImageView;
    class MenuItem : public View
    {
    public:
        MenuItem(Engine::BaseEngine& e, Menu& baseMenu, Daedalus::GameState::MenuItemHandle scriptHandle);
        ~MenuItem();

        void setTextLine(const std::string& text, unsigned int line = 0);

        /** 
         * Sets the highlighted font enabled or disabled. 
         * Note: Fonts must follow the original naming scheme with the "_HI" denoting a highlighted font.
         * @param value Whether to highlight this font
         */
        void setFontHighlighted(bool value);

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config) override;

        /**
         * @return Whether this item is selectable
         */
        bool isSelectable(){ return (getItemScriptData().flags & Daedalus::GEngineClasses::C_Menu_Item::IT_SELECTABLE) != 0; }
        /**
         * @return Script-data of this item
         */
        Daedalus::GEngineClasses::C_Menu_Item& getItemScriptData();

        /**
         * @returns the on-select action of this item (ie. Accept-key was pressed -> close view)
         */
        Daedalus::GEngineClasses::MenuConstants::ESelEvent getSelectionEvent(int i);
    protected:
        /**
         * Reference to the menu owning this
         */
        Menu& m_BaseMenu;

        /**
         * Handle to this items script-data
         */
        Daedalus::GameState::MenuItemHandle m_ScriptHandle;

        /**
         * Lines of text this view can render
         */
        std::vector<std::string> m_TextLines;

        /**
         * This menus background
         */
        ImageView* m_pBackgroundImage;
    };

    namespace MenuItemTypes
    {
        class MenuItemText : public MenuItem
        {
        public:
            MenuItemText(Engine::BaseEngine& e, Menu& baseMenu, const Daedalus::GameState::MenuItemHandle& scriptHandle);

            /**
             * Updates/draws the UI-Views
             * @param dt time since last frame
             * @param mstate mouse-state
             */
            void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config) override;
        };
    }
}
