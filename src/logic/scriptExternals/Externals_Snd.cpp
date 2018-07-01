#include "Externals_Snd.h"
#include <components/VobClasses.h>
#include <daedalus/DaedalusStdlib.h>
#include <daedalus/DaedalusVM.h>
#include <debugdraw/debugdraw.h>
#include <engine/GameEngine.h>
#include <logic/DialogManager.h>
#include <logic/PlayerController.h>
#include <logic/ScriptEngine.h>
#include <logic/visuals/ModelVisual.h>
#include <ui/PrintScreenMessages.h>
#include <utils/logger.h>

using namespace Logic;

void ScriptExternals::registerEngineExternals_snd(World::WorldInstance& world,
                                                  Daedalus::DaedalusVM* vm,
                                                  bool verbose)
{
    Engine::BaseEngine* engine = world.getEngine();
    World::WorldInstance* pWorld = &world;
    using Daedalus::GameState::ItemHandle;
    using Daedalus::GameState::NpcHandle;

    vm->registerExternalFunction("snd_play", [=](Daedalus::DaedalusVM& vm) {
        std::string s0 = vm.popString();

        pWorld->getAudioWorld().playSound(s0);
    });

    /*vm->registerExternalFunction("snd_getdisttosource", [=](Daedalus::DaedalusVM& vm){
      uint32_t arr_self;
      int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
      vm.setReturn(0);
      });*/
}
