#pragma once
#include "Menu.h"

namespace UI
{
class Menu_Settings : public Menu
{
public:
    Menu_Settings(Engine::BaseEngine& e);
    ~Menu_Settings();

    /**
         * Creates an instance of this class and appends it to the root UI-View
         * @return Instance of the class. Don't forget to delete!
         */
    static Menu_Settings* create(Engine::BaseEngine& e);

private:
};
}
