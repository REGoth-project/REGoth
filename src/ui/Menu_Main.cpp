
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

        std::string worldName = m_Engine.getEngineArgs().startupZEN;

        bool synchronous = true;
        auto policy = synchronous ? std::launch::deferred : std::launch::async;

        Engine::AsyncAction::JobType<void> addWorld = [worldName](Engine::BaseEngine* engine){

            Engine::AsyncAction::JobType<void> prolog = [](Engine::BaseEngine* engine) {
                engine->getHud().getLoadingScreen().setHidden(false);
                engine->resetSession();
            };
            auto prologFuture = engine->onMessage(prolog);
            // block this thread until the prolog gets started by the main thread
            while (prologFuture.wait_for(std::chrono::nanoseconds(0)) == std::future_status::deferred)
            {}
            // block this thread until the prolog epilog finished
            prologFuture.wait();

            std::shared_ptr<World::WorldInstance> sharedWorld;
            Engine::AsyncAction::JobType<std::shared_ptr<World::WorldInstance>> createWorld =
                    [worldName](Engine::BaseEngine* engine) {
                        return engine->getSession().createWorld(worldName);
                    };
            bool synchronous = true;
            if (synchronous)
            {
                auto future = engine->onMessage(createWorld);
                while (future.wait_for(std::chrono::nanoseconds(0)) == std::future_status::deferred)
                {std::this_thread::sleep_for(std::chrono::milliseconds(10));}
                sharedWorld = future.get();
            }
            else
                sharedWorld = createWorld(engine);

            Engine::AsyncAction::JobType<void> registerWorld = [sharedWorld](Engine::BaseEngine* engine){
                Handle::WorldHandle worldHandle = engine->getSession().registerWorld(sharedWorld);
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
            engine->onMessage(registerWorld);
            // no need to wait for registerWorld, this thread's work is done here
        };
        m_Engine.onMessage(addWorld, std::launch::async);

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
