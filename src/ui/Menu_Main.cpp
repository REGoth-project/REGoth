
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
    Engine::ExcludeFrameTime excludeFrameTime(m_Engine);
    using Engine::AsyncAction;
    using Engine::ExecutionPolicy;
    if(action == "NEW_GAME")
    {
        LogInfo() << "Starting new game...";
        getHud().popMenu();

        std::string worldName = m_Engine.getEngineArgs().startupZEN;

        Engine::AsyncAction::JobType<void> addWorld = [worldName](Engine::BaseEngine* engine){

            auto prolog = [](Engine::BaseEngine* engine) {
                engine->getHud().getLoadingScreen().setHidden(false);
                engine->resetSession();
            };
            AsyncAction::executeInThread(prolog, engine, ExecutionPolicy::MainThread).wait();

            std::unique_ptr<World::WorldInstance> uniqueWorld;
            bool synchronous = false;
            if (synchronous)
            {
                auto pUniqueWorld = &uniqueWorld;
                Engine::AsyncAction::JobType<void> createWorld =
                        [worldName, pUniqueWorld](Engine::BaseEngine* engine) {
                            *pUniqueWorld = engine->getSession().createWorld(worldName);
                        };
                AsyncAction::executeInThread(createWorld, engine, ExecutionPolicy::MainThread).wait();
            }
            else
                uniqueWorld = engine->getSession().createWorld(worldName);

            auto registerWorld = [w = std::move(uniqueWorld)](Engine::BaseEngine* engine) mutable {
                Handle::WorldHandle worldHandle = engine->getSession().registerWorld(std::move(w));
                if (worldHandle.isValid())
                {
                    engine->getSession().setMainWorld(worldHandle);
                    auto& se = worldHandle.get().getScriptEngine();
                    auto player = se.createDefaultPlayer(engine->getEngineArgs().playerScriptname);
                    worldHandle.get().takeControlOver(player);
                } else
                {
                    LogError() << "Failed to add given startup world, world handle is invalid!";
                }
                engine->getHud().getLoadingScreen().setHidden(true);
            };
            AsyncAction::executeInThread(std::move(registerWorld), engine, ExecutionPolicy::MainThread);
        };
        AsyncAction::executeInThread(addWorld, &m_Engine, ExecutionPolicy::NewThread);

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
