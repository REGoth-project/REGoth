//
// Created by desktop on 21.12.16.
//

#pragma once

#include <ZenLib/daedalus/DaedalusGameState.h>
#include "View.h"

namespace Daedalus
{
    class DaedalusVM;
}

namespace UI
{


    class MenuItem;
    class ImageView;
    class TextView;
    class Hud;
    class Menu : public View
    {
    public:
        Menu(Engine::BaseEngine& e);
        ~Menu();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config) override;

        /**
         * Initializes this menu from script. Also loads the MENU.DAT, if not already done.
         * @param instance Instance-name
         */
        void initializeInstance(const std::string& instance);

        /**
         * Scriptside data of the currently initialized menu.
         * Note: Make sure this class was initialized before calling this!
         * @return Data of the menu initialized here
         */
        Daedalus::GEngineClasses::C_Menu& getScriptData();

        /**
         * Looks up the script-data to the given item handle
         * @param item Menu item to look up
         * @return Reference to script-side data of the given item
         */
        Daedalus::GEngineClasses::C_Menu_Item& getItemScriptData(Daedalus::GameState::MenuItemHandle item);
        Daedalus::GEngineClasses::C_Menu_Item& getItemScriptData(const std::string& instance);

        /**
         * To be called when one of the given actions were triggered
         * @param action Input action
         */
        virtual void onInputAction(EInputAction action);


        /**
         * @return Underlaying HUD
         */
        Hud& getHud();

    protected:

        /**
         * Performs the select-action on the given item
         * @param item Item to perform the action on
         */
        virtual void performSelectAction(Daedalus::GameState::MenuItemHandle item);

        /**
         * Called on a custom select-action, comming from the "onSelAction"-field
         */
        virtual void onCustomAction(const std::string& action){};

        /**
         * Initializes the items registered at the script-data-entry
         * @return Map of handles to the created item-data and actual views
         */
        std::map<Daedalus::GameState::MenuItemHandle, MenuItem*> initializeItems();

        /**
         * Loads the MENU.DAT from the games files and initializes the VM
         * @return success
         */
        bool loadMenuDAT();

        /**
         * This menus DaedalusVM. These are quite small and every menu can have its own.
         */
        Daedalus::DaedalusVM* m_pVM;

        /**
         * Handle of the script-side data of this menu
         */
        Daedalus::GameState::MenuHandle m_MenuHandle;

        /**
         * Items created by this menu
         */
        std::map<Daedalus::GameState::MenuItemHandle, MenuItem*> m_Items;
        std::vector<Daedalus::GameState::MenuItemHandle> m_SelectableItems;

        /**
         * Currently selected item (Index into m_SelectableItems)
         */
        size_t m_SelectedItem;

        /**
         * This menus background
         */
        ImageView* m_pBackgroundImage;
        TextView* m_pInfoText; 
    };
}

