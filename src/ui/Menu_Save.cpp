#include "Menu_Save.h"
#include "MenuItem.h"
#include <engine/BaseEngine.h>
#include "Hud.h"
#include "Menu_Main.h"

using namespace UI;

Menu_Save::Menu_Save(Engine::BaseEngine& e) : Menu(e)
{
}

Menu_Save::~Menu_Save()
{
}

Menu_Save* Menu_Save::create(Engine::BaseEngine& e)
{
    Menu_Save* s = new Menu_Save(e);
    s->initializeInstance("MENU_SAVEGAME_SAVE");

    return s;
}

void Menu_Save::performSelectAction(Daedalus::GameState::MenuItemHandle item)
{
    MenuItem* iData = m_Items[item];

    using namespace Daedalus::GEngineClasses::MenuConstants;

    Menu::performSelectAction(item);
}
