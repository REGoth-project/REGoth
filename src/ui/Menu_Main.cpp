
#include <engine/BaseEngine.h>
#include <engine/Platform.h>
#include "Menu_Main.h"
#include <utils/logger.h>
#include "Hud.h"
#include "Menu_Load.h"
#include "Menu_Save.h"
#include "Menu_Settings.h"
#include <ui/LoadingScreen.h>

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
        auto prolog = [](Engine::BaseEngine& engine) -> bool {
            engine.getHud().getLoadingScreen().setHidden(false);
            engine.resetSession();
            return true;
        };
        prolog(m_Engine);
        Engine::BaseEngine* pEngine = &m_Engine;
        auto createWorld = [pEngine](){
            return pEngine->getSession().createWorld(pEngine->getEngineArgs().startupZEN);
        };
        bool synchronHack = true;
        auto mode = synchronHack ? std::launch::deferred : std::launch::async;
        std::shared_future<std::shared_ptr<World::WorldInstance>> worldFuture = std::async(mode,
                                                                                           createWorld);

        auto registerWorld = [worldFuture, synchronHack](Engine::BaseEngine& engine) -> bool {
            if (synchronHack)
                worldFuture.wait();
            // test if result is ready
            auto status = worldFuture.wait_for(std::chrono::nanoseconds(0));
            if (status != std::future_status::ready)
                return false;

            Handle::WorldHandle worldHandle = engine.getSession().registerWorld(worldFuture.get());
            if (worldHandle.isValid())
            {
                engine.getSession().setMainWorld(worldHandle);
                auto player = worldHandle.get().getScriptEngine().createDefaultPlayer(engine.getEngineArgs().playerScriptname);
                worldHandle.get().takeControlOver(player);
            } else
            {
                LogError() << "Failed to add given startup world, world handle is invalid!";
            }
            engine.getHud().getLoadingScreen().setHidden(true);
            return true;
        };
        m_Engine.onMessage(registerWorld);

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
