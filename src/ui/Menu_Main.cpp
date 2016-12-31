
#include <engine/BaseEngine.h>
#include "Menu_Main.h"
#include <utils/logger.h>

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
        case IA_Close: setHidden(true); break;
        case IA_Accept:break;
    }
}


void Menu_Main::onCustomAction(const std::string& action)
{
    if(action == "NEW_GAME")
    {
        LogInfo() << "Starting new game...";
        m_Engine.loadWorld(m_Engine.getEngineArgs().startupZEN);
    }
}
