
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

bool Menu_Main::onInputAction(EInputAction action)
{
    return Menu::onInputAction(action);
}


void Menu_Main::onCustomAction(const std::string& action)
{
    if(action == "NEW_GAME")
    {
        LogInfo() << "Starting new game...";
        getHud().popMenu();

        // TODO: extract method: clearSession(): invalidates slot index, remove active world, inactive worlds...
        Engine::SavegameManager::invalidateCurrentSlotIndex();
        m_Engine.removeAllWorlds();

        Handle::WorldHandle worldHandle = m_Engine.loadWorld(m_Engine.getEngineArgs().startupZEN);
        if (worldHandle.isValid())
        {
            m_Engine.setMainWorld(worldHandle);
            auto player = worldHandle.get().getScriptEngine().createDefaultPlayer("EBR_108_VELAYA");
            worldHandle.get().takeControlOver(player);
            // reset the clock to the default starting time
            m_Engine.getGameClock().resetNewGame();
        } else
        {
            LogError() << "Failed to add given startup world, world handle is invalid!";
        }

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

Daedalus::GameType Menu_Main::determineGameType() {
    bool isGothic2 = m_pVM->getDATFile().hasSymbolName("C_Menu_Item.hideOnValue");
    return isGothic2 ? Daedalus::GameType::GT_Gothic2 : Daedalus::GameType::GT_Gothic1;
}
