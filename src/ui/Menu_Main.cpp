
#include <engine/BaseEngine.h>
#include <engine/Platform.h>
#include "Menu_Main.h"
#include <utils/logger.h>
#include "Hud.h"
#include "Menu_Load.h"
#include "Menu_Save.h"
#include "Menu_Settings.h"

using namespace UI;



Menu_Main::Menu_Main(Engine::BaseEngine& e) : Menu(e)
{

}

Menu_Main* Menu_Main::create(Engine::BaseEngine& e)
{
    Menu_Main* s = new Menu_Main(e);
    s->initializeInstance("MENU_MAIN");

    return s;
}

void Menu_Main::onInputAction(EInputAction action)
{
    Menu::onInputAction(action);

    switch(action)
    {
        case IA_Up:break;
        case IA_Down:break;
        case IA_Left:break;
        case IA_Right:break;
        case IA_Close: getHud().popMenu(); break;
        case IA_Accept:break;
    }
}


void Menu_Main::onCustomAction(const std::string& action)
{
    if(action == "NEW_GAME")
    {
        getHud().popMenu();

        LogInfo() << "Starting new game...";
        m_Engine.loadWorld(m_Engine.getEngineArgs().startupZEN);
        // reset the clock to the default starting time
        m_Engine.getGameClock().resetNewGame();

    }else if(action == "MENU_SAVEGAME_LOAD")
    {
        getHud().pushMenu<Menu_Load>();
    }else if(action == "MENU_SAVEGAME_SAVE")
    {
        getHud().pushMenu<Menu_Save>();
    }else if(action == "MENU_OPTIONS")
    {
        getHud().pushMenu<Menu_Settings>();
    }else if(action == "MENU_LEAVE_GAME")
    {
        Engine::Platform::setQuit(true);
    }
}
