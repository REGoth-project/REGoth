//
// Created by andre on 03.06.16.
//

#include "Externals.h"
#include <components/VobClasses.h>
#include <daedalus/DaedalusStdlib.h>
#include <daedalus/DaedalusVM.h>
#include <debugdraw/debugdraw.h>
#include <engine/GameEngine.h>
#include <logic/DialogManager.h>
#include <logic/PlayerController.h>
#include <logic/ScriptEngine.h>
#include <logic/visuals/ModelVisual.h>
#include <ui/Hud.h>
#include <ui/IntroduceChapterView.h>
#include <ui/PrintScreenMessages.h>
#include <utils/logger.h>

#include "Externals_AI.h"
#include "Externals_Npc.h"
#include "Externals_Snd.h"
#include "Externals_Wld.h"

void ::Logic::ScriptExternals::registerEngineExternals(World::WorldInstance& world, Daedalus::DaedalusVM* vm, bool verbose)
{
    Engine::BaseEngine* engine = world.getEngine();
    World::WorldInstance* pWorld = &world;
    using Daedalus::GameState::ItemHandle;
    using Daedalus::GameState::NpcHandle;

    registerEngineExternals_ai(world, vm, verbose);
    registerEngineExternals_snd(world, vm, verbose);
    registerEngineExternals_wld(world, vm, verbose);
    registerEngineExternals_npc(world, vm, verbose);

    auto getNPCByInstance = [=](size_t instance) {
        return pWorld->getScriptEngine().findNPCVobFromScriptInstance(instance);
    };

    vm->registerExternalFunction("ta_min", [=](Daedalus::DaedalusVM& vm) {
        std::string waypoint = vm.popString();

        uint32_t action = vm.popDataValue();
        int32_t stop_m = vm.popDataValue();
        int32_t stop_h = vm.popDataValue();
        int32_t start_m = vm.popDataValue();
        int32_t start_h = vm.popDataValue();
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self);

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            npc.playerController->getAIStateMachine().insertRoutine(start_h, start_m, stop_h, stop_m, action, waypoint);
        }
    });

    /**
     * EquipItem
     */
    vm->registerExternalFunction("equipitem", [=](Daedalus::DaedalusVM& vm) {
        uint32_t instance = static_cast<uint32_t>(vm.popDataValue());
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            Daedalus::GameState::ItemHandle item = vm.getGameState().createInventoryItem(instance,
                                                                                         VobTypes::getScriptHandle(npc));
            VobTypes::NPC_EquipWeapon(npc, item);
        }
        else
        {
            //LogWarn() << "No userptr on npc: " << npcData.name[0];
        }
    });

    /**
     * GetDistTo...
     */
    vm->registerExternalFunction("printdebuginstch", [=](Daedalus::DaedalusVM& vm) {
        uint32_t arr_npc;
        std::string s = vm.popString();
        int32_t ch = vm.popDataValue();

        return;

        // Remove some really annoying debug-output
        if (s == "### (0) ### -> B_Scale")
            return;

        LogInfo() << "DEBUG: " << s;
    });

    vm->registerExternalFunction("printscreen", [=](Daedalus::DaedalusVM& vm) {
        int32_t timesec = vm.popDataValue();
        std::string font = vm.popString();
        int32_t posy = vm.popDataValue();
        int32_t posx = vm.popDataValue();
        std::string msg = vm.popString();
        int32_t dialognr = vm.popDataValue();

        pWorld->getPrintScreenManager().printMessageTimed(posx / 100.0f,
                                                          posy / 100.0f,
                                                          msg,
                                                          static_cast<double>(timesec));
    });

    vm->registerExternalFunction("hlp_getinstanceid", [=](Daedalus::DaedalusVM& vm) {
        int32_t sym = vm.popVar();

        // Lookup what's behind this symbol. Could be a reference!
        VobTypes::NpcVobInformation npc = getNPCByInstance(sym);

        if (npc.isValid())
        {
            vm.setReturn((int32_t)npc.playerController->getScriptInstance().instanceSymbol);
        }
        else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("infomanager_hasfinished", [=](Daedalus::DaedalusVM& vm) {
        vm.setReturn(pWorld->getDialogManager().isDialogActive() ? 0 : 1);
    });

    vm->registerExternalFunction("info_addchoice", [=](Daedalus::DaedalusVM& vm) {
        uint32_t func = vm.popVar();
        std::string text = vm.popString();
        uint32_t infoInstance = vm.popDataValue();

        Daedalus::GameState::InfoHandle hInfo = ZMemory::handleCast<Daedalus::GameState::InfoHandle>(
            pWorld->getScriptEngine().getVM().getDATFile().getSymbolByIndex(infoInstance).instanceDataHandle);

        auto& cInfo = vm.getGameState().getInfo(hInfo);
        cInfo.addChoice(Daedalus::GEngineClasses::SubChoice{text, func});
    });

    vm->registerExternalFunction("info_clearchoices", [=](Daedalus::DaedalusVM& vm) {
        uint32_t infoInstance = vm.popDataValue();

        Daedalus::GameState::InfoHandle hInfo = ZMemory::handleCast<Daedalus::GameState::InfoHandle>(
            vm.getDATFile().getSymbolByIndex(infoInstance).instanceDataHandle);

        auto& cInfo = vm.getGameState().getInfo(hInfo);
        cInfo.subChoices.clear();
    });

    vm->registerExternalFunction("mob_hasitems", [=](Daedalus::DaedalusVM& vm) {
        uint32_t iteminstance = (uint32_t)vm.popDataValue();
        std::string mobname = vm.popString();

        Handle::EntityHandle mob = VobTypes::MOB_GetByName(*pWorld, mobname);
        VobTypes::MobVobInformation mvob = VobTypes::asMobVob(*pWorld, mob);
        if (mvob.isValid())
        {
            int cnt = VobTypes::MOB_GetItemCount(mvob, vm.getDATFile().getSymbolByIndex(iteminstance).name);
            vm.setReturn(cnt);
        }
        else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("createinvitem", [=](Daedalus::DaedalusVM& vm) {
        uint32_t itemInstance = (uint32_t)vm.popDataValue();
        uint32_t arr_n0;
        int32_t npc = vm.popVar(arr_n0);

        NpcHandle hnpc = ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(npc).instanceDataHandle);

        vm.getGameState().createInventoryItem(itemInstance, hnpc);

        /*
        Daedalus::GEngineClasses::C_Npc& npcData = vm.getGameState().getNpc(hnpc);
        auto& parsym = vm.getDATFile().getSymbolByIndex(itemInstance);
        LogInfo() << "1: " << parsym.name;
        LogInfo() << "2. " << npcData.name[0];
        LogInfo() << " ##### Created Inventory-Item '" << parsym.name << "' for NPC: " << npcData.name[0];
         */
    });

    vm->registerExternalFunction("createinvitems", [=](Daedalus::DaedalusVM& vm) {
        uint32_t num = (uint32_t)vm.popDataValue();
        uint32_t itemInstance = (uint32_t)vm.popDataValue();
        uint32_t arr_n0;
        int32_t npc = vm.popVar(arr_n0);

        NpcHandle hnpc = ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(npc).instanceDataHandle);

        vm.getGameState().createInventoryItem(itemInstance, hnpc, num);
    });

    vm->registerExternalFunction("hlp_getnpc", [=](Daedalus::DaedalusVM& vm) {
        int32_t instancename = vm.popDataValue();

        /*if(!vm.getDATFile().getSymbolByIndex(instancename).instanceDataHandle.isValid())
        {

        }else
        {
            GEngineClasses::C_Npc& npcData = getNpc(ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(instancename).instanceDataHandle));
            if(l) LogInfo() << " [HLP_GETNPC] Name: "
                  << npcData.name[0];
        }*/

        vm.setReturnVar(instancename);
    });

    vm->registerExternalFunction("hlp_isvalidnpc", [=](Daedalus::DaedalusVM& vm) {
        int32_t self = vm.popVar();

        if (vm.getDATFile().getSymbolByIndex(self).instanceDataHandle.isValid())
        {
            vm.setReturn(1);
        }
        else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("Log_CreateTopic", [=](Daedalus::DaedalusVM& vm) {
        int32_t section = vm.popDataValue();
        std::string topicName = vm.popString();

        auto& logManager = engine->getSession().getLogManager();
        logManager.createTopic(topicName, static_cast<Daedalus::GameState::LogTopic::ESection>(section));
    });

    vm->registerExternalFunction("Log_SetTopicStatus", [=](Daedalus::DaedalusVM& vm) {
        int32_t status = vm.popDataValue();
        std::string topicName = vm.popString();

        auto& logManager = engine->getSession().getLogManager();
        logManager.setTopicStatus(topicName, static_cast<Daedalus::GameState::LogTopic::ELogStatus>(status));
    });

    vm->registerExternalFunction("Log_AddEntry", [=](Daedalus::DaedalusVM& vm) {
        std::string entry = vm.popString();
        std::string topicName = vm.popString();

        LogInfo() << "";
        LogInfo() << " ########### New Log Entry: " << topicName << " ########### ";
        LogInfo() << entry;
        LogInfo() << "";

        auto& logManager = engine->getSession().getLogManager();
        logManager.addEntry(topicName, entry);

        pWorld->getScriptEngine().onLogEntryAdded(topicName, entry);
    });

    vm->registerExternalFunction("inttostring", [](Daedalus::DaedalusVM& vm) {
        int32_t x = vm.popDataValue();

        vm.setReturn(std::to_string(x));
    });

    vm->registerExternalFunction("floattoint", [](Daedalus::DaedalusVM& vm) {
        int32_t x = vm.popDataValue();
        float f = reinterpret_cast<float&>(x);
        vm.setReturn(static_cast<int32_t>(f));
    });

    vm->registerExternalFunction("inttofloat", [](Daedalus::DaedalusVM& vm) {
        int32_t x = vm.popDataValue();
        float f = static_cast<float>(x);
        vm.setReturn(reinterpret_cast<int32_t&>(f));
    });

    vm->registerExternalFunction("concatstrings", [](Daedalus::DaedalusVM& vm) {
        std::string s2 = vm.popString();
        std::string s1 = vm.popString();

        vm.setReturn(s1 + s2);
    });

    vm->registerExternalFunction("hlp_strcmp", [](Daedalus::DaedalusVM& vm) {
        std::string s1 = vm.popString();
        std::string s2 = vm.popString();

        vm.setReturn(s1 == s2 ? 1 : 0);
    });

    vm->registerExternalFunction("hlp_random", [=](Daedalus::DaedalusVM& vm) {
        int32_t n0 = vm.popDataValue();

        vm.setReturn(rand() % n0);
    });

    vm->registerExternalFunction("introducechapter", [=](Daedalus::DaedalusVM& vm) {
        double waittime = vm.popDataValue();
        std::string sound = vm.popString();
        std::string texture = vm.popString();
        std::string subtitle = vm.popString();
        std::string title = vm.popString();

        LogInfo() << "Queued up chapter introduction " << title;

        engine->getJobManager().executeInMainThread<void>([=](Engine::BaseEngine* pEngine) {
            engine->getHud().getIntroduceChapterView().enqueueChapterIntroduction(title, subtitle, texture, sound, waittime);
        });
    });
}
