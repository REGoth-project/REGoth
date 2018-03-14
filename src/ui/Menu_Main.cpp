
#include "Menu_Main.h"
#include "Hud.h"
#include "Menu_Load.h"
#include "Menu_Save.h"
#include "Menu_Settings.h"
#include <engine/BaseEngine.h>
#include <engine/Platform.h>
#include <ui/LoadingScreen.h>
#include <utils/logger.h>
#include <logic/ScriptEngine.h>

using namespace UI;

Menu_Main::Menu_Main(Engine::BaseEngine& e)
    : Menu(e)
{
}

Menu_Main* Menu_Main::create(Engine::BaseEngine& e)
{
    Menu_Main* s = new Menu_Main(e);
    s->initializeInstance("MENU_MAIN");

    return s;
}

bool Menu_Main::onInputAction(Engine::ActionType action)
{
    /**
     * Ignore UI_Close, since when a key both bound to UI_Close and UI_ToggleMainMenu,
     * like escape, is pressed, the menu will be immediately re-opened after closing.
     */
    if (action == Engine::ActionType::UI_Close)
        return false;
    else if (action == Engine::ActionType::UI_ToggleMainMenu)
        return true;
    return Menu::onInputAction(action);
}

void Menu_Main::onCustomAction(const std::string& action)
{
    Utils::RecursiveStopWatch excludeFrameTime(m_Engine.m_ExcludedFrameTime);
    if (action == "NEW_GAME")
    {
        LogInfo() << "Starting new game...";
        getHud().popMenu();
        m_Engine.getSession().startNewGame(m_Engine.getEngineArgs().startupZEN);
    }
    else if (action == "MENU_SAVEGAME_LOAD")
    {
        getHud().pushMenu<Menu_Load>();
    }
    else if (action == "MENU_SAVEGAME_SAVE")
    {
        getHud().pushMenu<Menu_Save>();
    }
    else if (action == "MENU_OPTIONS")
    {
        getHud().pushMenu<Menu_Settings>();
    }
    else if (action == "MENU_LEAVE_GAME")
    {
        Engine::Platform::setQuit(true);
    }
}

Daedalus::GameType Menu_Main::determineGameType()
{
    bool isGothic2 = m_pVM->getDATFile().hasSymbolName("C_Menu_Item.hideOnValue");
    return isGothic2 ? Daedalus::GameType::GT_Gothic2 : Daedalus::GameType::GT_Gothic1;
}
