#include "Menu_Settings.h"

using namespace UI;

Menu_Settings::Menu_Settings(Engine::BaseEngine& e)
    : Menu(e)
{
}

Menu_Settings* Menu_Settings::create(Engine::BaseEngine& e)
{
    Menu_Settings* s = new Menu_Settings(e);
    s->initializeInstance("MENU_OPTIONS");

    return s;
}
