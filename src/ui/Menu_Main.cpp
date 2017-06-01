
#include <engine/BaseEngine.h>
#include <engine/Platform.h>
#include "Menu_Main.h"
#include <utils/logger.h>
#include "Hud.h"
#include "Menu_Load.h"
#include "Menu_Save.h"
#include "Menu_Settings.h"
#include <ui/LoadingScreen.h>
#include "engine/AsyncAction.h"

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
    using Engine::AsyncAction;
    if(action == "NEW_GAME")
    {
        LogInfo() << "Starting new game...";
        getHud().popMenu();

        std::string worldName = m_Engine.getEngineArgs().startupZEN;

        bool synchronous = true;
        auto policy = synchronous ? std::launch::deferred : std::launch::async;

        Engine::AsyncAction::JobType<void> addWorld = [worldName](Engine::BaseEngine* engine){

            auto prolog = [](Engine::BaseEngine* engine) {
                engine->getHud().getLoadingScreen().setHidden(false);
                engine->resetSession();
            };
            auto prologFuture = AsyncAction::onMessage(prolog, engine);
            // block this thread until the prolog gets started by the main thread
            while (prologFuture.wait_for(std::chrono::nanoseconds(0)) == std::future_status::deferred)
            {}
            // block this thread until the prolog epilog finished
            prologFuture.wait();

            std::unique_ptr<World::WorldInstance> uniqueWorld;
            bool synchronous = true;
            if (synchronous)
            {
                auto pUniqueWorld = &uniqueWorld;
                Engine::AsyncAction::JobType<void> createWorld =
                        [worldName, pUniqueWorld](Engine::BaseEngine* engine) {
                            *pUniqueWorld = engine->getSession().createWorld(worldName);
                        };
                auto future =AsyncAction::onMessage(createWorld, engine);
                while (future.wait_for(std::chrono::nanoseconds(0)) == std::future_status::deferred)
                {std::this_thread::sleep_for(std::chrono::milliseconds(10));}
                future.wait();
            }
            else
                uniqueWorld = engine->getSession().createWorld(worldName);

            auto registerWorld = [w = std::move(uniqueWorld)](Engine::BaseEngine* engine) mutable {
                Handle::WorldHandle worldHandle = engine->getSession().registerWorld(std::move(w));
                if (worldHandle.isValid())
                {
                    engine->getSession().setMainWorld(worldHandle);
                    auto player = worldHandle.get().getScriptEngine().createDefaultPlayer(engine->getEngineArgs().playerScriptname);
                    worldHandle.get().takeControlOver(player);
                } else
                {
                    LogError() << "Failed to add given startup world, world handle is invalid!";
                }
                engine->getHud().getLoadingScreen().setHidden(true);
            };
            auto registerWorldFuture = AsyncAction::onMessage(std::move(registerWorld), engine);
        };
        AsyncAction::onMessage(addWorld, &m_Engine, std::launch::async);

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
