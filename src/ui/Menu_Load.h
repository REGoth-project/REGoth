#pragma once
#include "Menu.h"

namespace UI
{
    class Menu_Load : public Menu
    {
    public:
        Menu_Load(Engine::GameEngine& e);
        ~Menu_Load();

        /**
         * Creates an instance of this class and appends it to the root UI-View
         * @return Instance of the class. Don't forget to delete!
         */
        static Menu_Load* create(Engine::GameEngine& e);

        /**
         * Gathers all available savegames and writes their names to the slot-labels
         */
        void gatherAvailableSavegames();

        void onCustomAction(const std::string& action);

        static constexpr auto const EMPTY_SLOT_DISPLAYNAME = "---";

    private:
    };
}
