#include "Menu_Load.h"

using namespace UI;

Menu_Load::Menu_Load(Engine::BaseEngine& e) : Menu(e)
{
}

Menu_Load::~Menu_Load()
{
}

Menu_Load* Menu_Load::create(Engine::BaseEngine& e)
{
    Menu_Load* s = new Menu_Load(e);
    s->initializeInstance("MENU_SAVEGAME_LOAD");

    return s;
}
