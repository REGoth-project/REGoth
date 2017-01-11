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
    private:

        /**
         * Performs the select-action on the given item
         * @param item Item to perform the action on
         */
        virtual void performSelectAction(Daedalus::GameState::MenuItemHandle item) override;
    };
}
