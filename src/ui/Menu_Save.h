#pragma once
#include "Menu.h"

namespace UI
{
    class Menu_Save : public Menu
    {
    public:
        Menu_Save(Engine::BaseEngine& e);
        ~Menu_Save();

        /**
         * Creates an instance of this class and appends it to the root UI-View
         * @return Instance of the class. Don't forget to delete!
         */
        static Menu_Save* create(Engine::BaseEngine& e);

        /**
         * Gathers all available savegames and writes their names to the slot-labels
         */
        void gatherAvailableSavegames();

        void onCustomAction(const std::string& action);

        /**
         * To be called when one of the given actions were triggered
         * @param action Input action
         */
        virtual bool onInputAction(EInputAction action) override;

        /**
         * To be called when there was text input since the last frame
         * @param text Characters input since the last frame
         */
        virtual void onTextInput(const std::string& text) override;

        static constexpr auto const EMPTY_SLOT_DISPLAYNAME = "---";

    private:
        /**
         * Performs the select-action on the given item
         * @param item Item to perform the action on
         */
        virtual void performSelectAction(Daedalus::GameState::MenuItemHandle item) override;

        bool m_isWaitingForSaveName = false;

        std::string m_SaveName;

        std::string m_MenuItemSaveSlot;
    };
}
