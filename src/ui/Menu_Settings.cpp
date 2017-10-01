#include "Menu_Settings.h"

using namespace UI;

Menu_Settings::Menu_Settings(Engine::GameEngine& e)
    : Menu(e)
{
}

Menu_Settings::~Menu_Settings()
{
}

Menu_Settings* Menu_Settings::create(Engine::GameEngine& e)
{
    Menu_Settings* s = new Menu_Settings(e);
    s->initializeInstance("MENU_OPTIONS");

    return s;
}
