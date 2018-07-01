#include "Externals_Npc.h"
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

void ScriptExternals::registerEngineExternals_npc(World::WorldInstance& world,
                                                  Daedalus::DaedalusVM* vm,
                                                  bool verbose)
{
    Engine::BaseEngine* engine = world.getEngine();
    World::WorldInstance* pWorld = &world;
    using Daedalus::GameState::ItemHandle;
    using Daedalus::GameState::NpcHandle;

    auto isSymInstanceValid = [vm](size_t instance) {
        return vm->getDATFile().getSymbolByIndex(instance).instanceDataHandle.isValid();
    };

    auto getNPCByInstance = [=](size_t instance) {
        return pWorld->getScriptEngine().findNPCVobFromScriptInstance(instance);
    };

    auto getItemByInstance = [=](size_t instance) {
        return pWorld->getScriptEngine().findItemVobFromScriptInstance(instance);
    };

    vm->registerExternalFunction("npc_getdisttonpc", [=](Daedalus::DaedalusVM& vm) {
        uint32_t arr_npc2;
        uint32_t npc2 = vm.popVar(arr_npc2);
        if (verbose) LogInfo() << "npc2: " << npc2;
        uint32_t arr_npc1;
        uint32_t npc1 = vm.popVar(arr_npc1);
        if (verbose) LogInfo() << "npc1: " << npc1;

        if (!isSymInstanceValid(npc1) || !isSymInstanceValid(npc2))
            vm.setReturn(INT32_MAX);

        VobTypes::NpcVobInformation vob1 = getNPCByInstance(npc1);
        VobTypes::NpcVobInformation vob2 = getNPCByInstance(npc2);

        if (vob1.isValid() && vob2.isValid())
        {
            // Calculate distance
            float dist = (Vob::getTransform(vob1).Translation() - Vob::getTransform(vob2).Translation()).length();

            // Convert to cm
            dist *= 100.0f;

            vm.setReturn(static_cast<int32_t>(dist));
        }
        else
        {
            vm.setReturn(INT32_MAX);
        }
    });

    vm->registerExternalFunction("npc_getdisttowp", [=](Daedalus::DaedalusVM& vm) {
        std::string wpname = vm.popString();
        if (verbose) LogInfo() << "wpname: " << wpname;
        uint32_t arr_self;
        uint32_t self = vm.popVar(arr_self);
        if (verbose) LogInfo() << "self: " << self;

        VobTypes::NpcVobInformation vob = getNPCByInstance(self);

        // Script could call this using an invalid instance
        if (!vob.isValid())
        {
            vm.setReturn(INT32_MAX);
            return;
        }

        // Calculate distance
        Math::Matrix selfTransform = Vob::getTransform(vob);
        World::WorldInstance& world = Vob::getWorld(vob);
        size_t wpidx = World::Waynet::getWaypointIndex(world.getWaynet(), wpname);

        if (wpidx != World::Waynet::INVALID_WAYPOINT)
        {
            float dist = (selfTransform.Translation() - world.getWaynet().waypoints[wpidx].position).length();

            // Convert to cm
            dist *= 100.0f;

            vm.setReturn(static_cast<int32_t>(dist));
        }
        else
        {
            vm.setReturn(INT32_MAX);
        }
    });

    vm->registerExternalFunction("npc_getdisttoitem", [=](Daedalus::DaedalusVM& vm) {
        uint32_t item = vm.popVar();
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc);
        if (verbose) LogInfo() << "npc: " << npc;

        VobTypes::NpcVobInformation npcvob = getNPCByInstance(npc);
        Vob::VobInformation itemvob = getItemByInstance(item);

        if (!itemvob.isValid())
        {
            vm.setReturn(INT32_MAX);
            return;
        }

        float dist = (Vob::getTransform(npcvob).Translation() - Vob::getTransform(itemvob).Translation()).length();

        // Convert to cm
        dist *= 100.0f;

        vm.setReturn(static_cast<int32_t>(dist));
    });

    vm->registerExternalFunction("npc_getdisttoplayer", [=](Daedalus::DaedalusVM& vm) {
        uint32_t arr_npc1;
        uint32_t npc1 = vm.popVar(arr_npc1);
        if (verbose) LogInfo() << "npc1: " << npc1;

        VobTypes::NpcVobInformation vob1 = getNPCByInstance(npc1);
        VobTypes::NpcVobInformation vob2 = getNPCByInstance(vm.getDATFile().getSymbolIndexByName("hero"));

        // Calculate distance
        float dist = (Vob::getTransform(vob1).Translation() - Vob::getTransform(vob2).Translation()).length();

        // Convert to cm
        dist *= 100.0f;

        vm.setReturn(static_cast<int32_t>(dist));
    });

    vm->registerExternalFunction("npc_isplayer", [=](Daedalus::DaedalusVM& vm) {
        uint32_t player = vm.popVar();
        if (verbose) LogInfo() << "player: " << player;

        VobTypes::NpcVobInformation npc = getNPCByInstance(player);

        if (npc.world && npc.isValid())
        {
            int r = npc.world->getScriptEngine().getPlayerEntity() == npc.entity;
            vm.setReturn(r);
        }
        else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("npc_canseenpc", [=](Daedalus::DaedalusVM& vm) {
        uint32_t other = vm.popVar();
        uint32_t self = vm.popVar();

        if (!isSymInstanceValid(other) || !isSymInstanceValid(self))
        {
            vm.setReturn(0);
            return;
        }

        VobTypes::NpcVobInformation vnpc1 = getNPCByInstance(self);
        VobTypes::NpcVobInformation vnpc2 = getNPCByInstance(other);

        if (vnpc1.isValid() && vnpc2.isValid())
            vm.setReturn(vnpc1.playerController->canSee(vnpc2.entity) ? 1 : 0);
        else
            vm.setReturn(0);
    });

    vm->registerExternalFunction("npc_canseenpcfreelos", [=](Daedalus::DaedalusVM& vm) {
        uint32_t other = vm.popVar();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation vnpc1 = getNPCByInstance(self);
        VobTypes::NpcVobInformation vnpc2 = getNPCByInstance(other);

        if (vnpc1.isValid() && vnpc2.isValid())
            vm.setReturn(vnpc1.playerController->canSee(vnpc2.entity, false) ? 1 : 0);
        else
            vm.setReturn(0);
    });

    vm->registerExternalFunction("npc_canseeitem", [=](Daedalus::DaedalusVM& vm) {
        uint32_t other = vm.popVar();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation vnpc1 = getNPCByInstance(self);
        Vob::VobInformation vitem = getItemByInstance(other);

        if (vnpc1.isValid() && vitem.isValid())
            vm.setReturn(vnpc1.playerController->canSee(vitem.entity) ? 1 : 0);
        else
            vm.setReturn(0);
    });

    vm->registerExternalFunction("npc_clearaiqueue", [=](Daedalus::DaedalusVM& vm) {
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
            npc.playerController->getEM().clear();
    });

    vm->registerExternalFunction("npc_exchangeroutine", [=](Daedalus::DaedalusVM& vm) {
        std::string routinename = vm.popString();
        if (verbose) LogInfo() << "routinename: " << routinename;
        uint32_t arr_self;
        uint32_t self = vm.popVar(arr_self);
        if (verbose) LogInfo() << "self: " << self;

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            npc.playerController->changeRoutine(routinename);
        }
    });

    vm->registerExternalFunction("npc_getnearestwp", [=](Daedalus::DaedalusVM& vm) {
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self);
        if (verbose) LogInfo() << "self: " << self;

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            size_t wp = npc.playerController->getClosestWaypoint();

            if (wp != World::Waynet::INVALID_WAYPOINT)
            {
                vm.setReturn(pWorld->getWaynet().waypoints[wp].name);
                return;
            }
        }

        vm.setReturn("");
    });

    vm->registerExternalFunction("npc_getnextwp", [=](Daedalus::DaedalusVM& vm) {
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self);
        if (verbose) LogInfo() << "self: " << self;

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            size_t wp = npc.playerController->getSecondClosestWaypoint();

            if (wp != World::Waynet::INVALID_WAYPOINT)
            {
                vm.setReturn(pWorld->getWaynet().waypoints[wp].name);
                return;
            }
        }

        vm.setReturn("");
    });

    vm->registerExternalFunction("npc_hasitems", [=](Daedalus::DaedalusVM& vm) {
        uint32_t iteminstance = vm.popDataValue();
        int32_t owner = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(owner);

        if (npc.isValid())
        {
            int count = npc.playerController->getInventory().getItemCount(iteminstance);
            vm.setReturn(count);
        }
        else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("npc_removeinvitem", [=](Daedalus::DaedalusVM& vm) {
        uint32_t iteminstance = vm.popDataValue();
        uint32_t owner = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(owner);

        if (npc.isValid())
        {
            int count = npc.playerController->getInventory().removeItem(iteminstance);
        }

        vm.setReturn(0);
    });

    vm->registerExternalFunction("npc_removeinvitems", [=](Daedalus::DaedalusVM& vm) {
        uint32_t amount = vm.popDataValue();
        uint32_t iteminstance = vm.popDataValue();
        uint32_t owner = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(owner);

        if (npc.isValid())
        {
            int count = npc.playerController->getInventory().removeItem(iteminstance, amount);
        }

        vm.setReturn(0);
    });

    vm->registerExternalFunction("npc_getstatetime", [=](Daedalus::DaedalusVM& vm) {
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            float time = npc.playerController->getAIStateMachine().getCurrentStateTime();
            vm.setReturn(static_cast<int>(time));
        }
        else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("npc_setstatetime", [=](Daedalus::DaedalusVM& vm) {
        if (verbose) LogInfo() << "npc_setstatetime";
        int seconds = vm.popDataValue();
        if (verbose) LogInfo() << "seconds: " << seconds;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self);
        if (verbose) LogInfo() << "self: " << self;

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);
        npc.playerController->getAIStateMachine().setCurrentStateTime(seconds);
    });

    vm->registerExternalFunction("npc_isdead", [=](Daedalus::DaedalusVM& vm) {
        int32_t n = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(n);

        if (npc.isValid())
        {
            vm.setReturn(npc.playerController->getBodyState() == BS_DEAD ? 1 : 0);
        }
        else
        {
            vm.setReturn(1);  // Default to "Yes, he's dead" in case he wasn't even loaded
        }
    });

    vm->registerExternalFunction("npc_isonfp", [=](Daedalus::DaedalusVM& vm) {
        std::string fpname = vm.popString(true);
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            vm.setReturn(1);
            // Find fp with given name
            // TODO
            /*Handle::EntityHandle fp = (*pWorld->getFreepoints().find(fpname)).second;
            Components::SpotComponent& sp = pWorld->getEntity<Components::SpotComponent>(fp);

            vm.setReturn(sp.m_UsingEntity == npc.entity && sp.m_UseEndTime < pWorld->getWorldInfo().time);*/
        }
        else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("npc_gettrueguild", [=](Daedalus::DaedalusVM& vm) {
        int32_t n = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(n);

        if (npc.isValid())
        {
            // TODO: Add true guild field to C_NPC and use it instead
            vm.setReturn(npc.playerController->getScriptInstance().guild);
        }
        else
        {
            vm.setReturn(vm.getDATFile().getSymbolByName("GIL_NONE").getInt());
        }
    });

    vm->registerExternalFunction("npc_settrueguild", [=](Daedalus::DaedalusVM& vm) {
        int32_t guild = vm.popDataValue();
        int32_t n = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(n);

        npc.playerController->getScriptInstance().guild = guild;
        vm.setReturn(0);
    });

    vm->registerExternalFunction("npc_checkinfo", [=](Daedalus::DaedalusVM& vm) {
        int important = vm.popDataValue();
        int32_t npc = vm.popVar();
        NpcHandle npcHandle = ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(npc).instanceDataHandle);
        bool hasInfos = pWorld->getDialogManager().checkInfo(npcHandle, important);
        vm.setReturn(hasInfos);
    });

    vm->registerExternalFunction("npc_changeattribute", [=](Daedalus::DaedalusVM& vm) {
        int32_t value = vm.popDataValue();
        int32_t atr = vm.popDataValue();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            npc.playerController->changeAttribute((Daedalus::GEngineClasses::C_Npc::EAttributes)atr, value);
        }
    });

    vm->registerExternalFunction("npc_giveitem", [=](Daedalus::DaedalusVM& vm) {
        if (verbose) LogInfo() << "npc_giveitem";

        uint32_t fromNpcId = vm.popVar();
        if (verbose) LogInfo() << "from" << fromNpcId;
        uint32_t itemInstance = vm.popVar();
        if (verbose) LogInfo() << "item" << itemInstance;
        uint32_t toNpcId = vm.popVar();
        if (verbose) LogInfo() << "to" << toNpcId;

        VobTypes::NpcVobInformation fromNpc = getNPCByInstance(fromNpcId);
        VobTypes::NpcVobInformation toNpc = getNPCByInstance(toNpcId);

        if (!fromNpc.isValid() || !toNpc.isValid())
            return;
        if (!fromNpc.playerController->getInventory().removeItem(itemInstance))
            return;

        toNpc.playerController->getInventory().addItem(itemInstance);
    });

    vm->registerExternalFunction("npc_clearinventory", [=](Daedalus::DaedalusVM& vm) {
        uint32_t npcId = vm.popVar();
        if (verbose) LogInfo() << "npc_clearinventory " << npcId;

        VobTypes::NpcVobInformation npc = getNPCByInstance(npcId);

        if (!npc.isValid())
            return;
        npc.playerController->getInventory().clear();
    });

    vm->registerExternalFunction("npc_setrefusetalk", [=](Daedalus::DaedalusVM& vm) {
        // the self argument is the NPC, the player is talking with
        int32_t timeSec = vm.popDataValue();
        uint32_t self = vm.popVar();
        VobTypes::NpcVobInformation npc = getNPCByInstance(self);
        npc.playerController->setRefuseTalkTime(timeSec);
    });

    vm->registerExternalFunction("npc_refusetalk", [=](Daedalus::DaedalusVM& vm) {
        // the self argument is the NPC, the player is talking with
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);
        int32_t isRefusingTalk = npc.playerController->isRefusingTalk();
        vm.setReturn(isRefusingTalk);
    });

    vm->registerExternalFunction("npc_isinstate", [=](Daedalus::DaedalusVM& vm) {
        if (verbose) LogInfo() << "npc_isinstate";
        uint32_t state = (uint32_t)vm.popVar();
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (!npc.isValid())
            return;

        int v = npc.playerController->getAIStateMachine().isInState(state) ? 1 : 0;

        vm.setReturn(v);
    });

    vm->registerExternalFunction("npc_hasequippedmeleeweapon", [=](Daedalus::DaedalusVM& vm) {
        if (verbose) LogInfo() << "npc_hasequippedmeleeweapon";
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (!npc.isValid())
        {
            vm.setReturn(0);
            return;
        }

        vm.setReturn(npc.playerController->hasEquippedMeleeWeapon());
    });

    vm->registerExternalFunction("npc_knowsinfo", [=](Daedalus::DaedalusVM& vm) {
        int32_t infoinstance = vm.popDataValue();
        int32_t self = vm.popVar();

        NpcHandle hself = ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(self).instanceDataHandle);
        Daedalus::GEngineClasses::C_Npc& npc = vm.getGameState().getNpc(hself);

        auto& scriptDialogManager = *pWorld->getDialogManager().getScriptDialogManager();
        bool knows = scriptDialogManager.doesNpcKnowInfo(npc.instanceSymbol, infoinstance);

        //LogInfo() << "Does he kow? (" << vm.getDATFile().getSymbolByIndex(npc.instanceSymbol).name << " -> " << vm.getDATFile().getSymbolByIndex(infoinstance).name << "): " << knows;

        vm.setReturn(knows);
    });

    vm->registerExternalFunction("npc_settofightmode", [=](Daedalus::DaedalusVM& vm) {
        size_t weaponSymbol = (size_t)vm.popDataValue();
        size_t self = (size_t)vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            // Give the specified item to the npc if he doesn't already have it.
            if (!npc.playerController->getInventory().getItemCount(weaponSymbol))
            {
                npc.playerController->getInventory().addItem(weaponSymbol);
            }

            // FIXME: This draws whatever the NPC has currently equipped
            npc.playerController->drawWeaponMelee();
        }
    });

    vm->registerExternalFunction("npc_settofistmode", [=](Daedalus::DaedalusVM& vm) {
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self);

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            npc.playerController->drawWeaponMelee(true);
        }
    });
}
