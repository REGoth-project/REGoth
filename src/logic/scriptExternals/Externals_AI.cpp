#include "Externals_AI.h"
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

void ScriptExternals::registerEngineExternals_ai(World::WorldInstance& world, Daedalus::DaedalusVM* vm, bool verbose)
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

    vm->registerExternalFunction("ai_teleport", [=](Daedalus::DaedalusVM& vm) {
        std::string waypoint = vm.popString();
        int32_t self = vm.popVar();

        const auto waypointIndex = World::Waynet::getWaypointIndex(pWorld->getWaynet(), waypoint);
        VobTypes::NpcVobInformation npcVob = getNPCByInstance(self);
        if (waypointIndex != World::Waynet::INVALID_WAYPOINT || npcVob.isValid())
            npcVob.playerController->teleportToWaypoint(waypointIndex);
    });

    vm->registerExternalFunction("ai_turntonpc", [=](Daedalus::DaedalusVM& vm) {
        uint32_t arr_n1;
        int32_t target = vm.popVar(arr_n1);
        if (verbose) LogInfo() << "target: " << target;
        uint32_t arr_n0;
        int32_t self = vm.popVar(arr_n0);
        if (verbose) LogInfo() << "self: " << self;

        if (!isSymInstanceValid(self) || !isSymInstanceValid(target))
            return;

        VobTypes::NpcVobInformation selfvob = getNPCByInstance(self);
        VobTypes::NpcVobInformation targetvob = getNPCByInstance(target);

        // Fill turn message
        EventMessages::MovementMessage msg;
        msg.subType = EventMessages::MovementMessage::ST_TurnToVob;
        msg.targetVob = targetvob.entity;

        // Push the message
        selfvob.playerController->getEM().onMessage(msg);
    });

    vm->registerExternalFunction("ai_standup", [=](Daedalus::DaedalusVM& vm) {
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            // Fill standup message
            EventMessages::MovementMessage msg;
            msg.subType = EventMessages::MovementMessage::ST_Standup;
            msg.targetMode = 1;  // Play transition-anis

            // Push the message
            npc.playerController->getEM().onMessage(msg);
        }
    });

    vm->registerExternalFunction("ai_standupquick", [=](Daedalus::DaedalusVM& vm) {
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            // Fill standup message
            EventMessages::MovementMessage msg;
            msg.subType = EventMessages::MovementMessage::ST_Standup;
            msg.targetMode = 0;  // No transitions

            // Push the message
            npc.playerController->getEM().onMessage(msg);
        }
    });

    vm->registerExternalFunction("ai_gotowp", [=](Daedalus::DaedalusVM& vm) {
        std::string wp = vm.popString();
        int32_t self = vm.popVar();

        if (!World::Waynet::waypointExists(pWorld->getWaynet(), wp))
            return;

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            EventMessages::MovementMessage sm;
            sm.subType = EventMessages::MovementMessage::ST_GotoVob;
            sm.targetVobName = wp;

            npc.playerController->getEM().onMessage(sm);
            //npc.playerController->gotoWaypoint(World::Waynet::getWaypointIndex(pWorld->getWaynet(), wp));
        }
    });

    vm->registerExternalFunction("ai_gotonextfp", [=](Daedalus::DaedalusVM& vm) {
        std::string fpname = vm.popString(true);
        int32_t self = vm.popVar();

        // FIXME: Same as ai_gotofp. What's the exact difference between them?
        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            // Find closest fp
            std::vector<Handle::EntityHandle> fp = pWorld->getFreepointsInRange(npc.position->m_WorldMatrix.Translation(), 20.0f, fpname, true);

            if (!fp.empty())
            {
                EventMessages::MovementMessage sm;
                sm.subType = EventMessages::MovementMessage::ST_GotoFP;
                sm.targetVob = fp.front();
                npc.playerController->getEM().onMessage(sm);
            }
        }
    });

    vm->registerExternalFunction("ai_gotofp", [=](Daedalus::DaedalusVM& vm) {
        std::string fpname = vm.popString(true);
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            // Find closest fp
            std::vector<Handle::EntityHandle> fp = pWorld->getFreepointsInRange(npc.position->m_WorldMatrix.Translation(), 20.0f, fpname, true);

            if (!fp.empty())
            {
                EventMessages::MovementMessage sm;
                sm.subType = EventMessages::MovementMessage::ST_GotoFP;
                sm.targetVob = fp.front();
                npc.playerController->getEM().onMessage(sm);
            }
        }
    });

    vm->registerExternalFunction("ai_gotonpc", [=](Daedalus::DaedalusVM& vm) {
        uint32_t other = vm.popVar();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation nself = getNPCByInstance(self);
        VobTypes::NpcVobInformation nother = getNPCByInstance(other);

        if (nself.isValid())
        {
            EventMessages::MovementMessage sm;
            sm.subType = EventMessages::MovementMessage::ST_GotoVob;
            sm.targetVob = nother.entity;

            nself.playerController->getEM().onMessage(sm);
            //npc.playerController->gotoWaypoint(World::Waynet::getWaypointIndex(pWorld->getWaynet(), wp));
        }
    });

    vm->registerExternalFunction("ai_startstate", [=](Daedalus::DaedalusVM& vm) {
        std::string wpname = vm.popString();
        int32_t statebehaviour = vm.popDataValue();
        uint32_t fnSym = vm.popVar();
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            EventMessages::StateMessage sm;
            sm.subType = EventMessages::StateMessage::EV_StartState;
            sm.wpname = wpname;
            sm.functionSymbol = fnSym;
            sm.other = pWorld->getScriptEngine().getNPCFromSymbol("other");
            sm.victim = pWorld->getScriptEngine().getNPCFromSymbol("victim");

            npc.playerController->getEM().onMessage(sm);
        }
    });

    vm->registerExternalFunction("ai_wait", [=](Daedalus::DaedalusVM& vm) {
        float duration = vm.popFloatValue();
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            EventMessages::StateMessage sm;
            sm.subType = EventMessages::StateMessage::EV_Wait;
            sm.waitTime = duration;

            npc.playerController->getEM().onMessage(sm);
        }
    });

    vm->registerExternalFunction("ai_playani", [=](Daedalus::DaedalusVM& vm) {
        std::string ani = vm.popString();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            EventMessages::ConversationMessage sm;
            sm.subType = EventMessages::ConversationMessage::ST_PlayAni;
            sm.animation = ani;

            // Don't schedule random animations when the npc is the target of the dialog manager
            // Should not be necessary since npc should be in ZS_TALK state. Remove when state issue is resolved
            NpcHandle npcHandle = npc.playerController->getScriptHandle();
            auto& dialogManager = pWorld->getDialogManager();
            if (dialogManager.isDialogActive() && dialogManager.getTarget() == npcHandle)
            {
                //LogInfo() << "Animation got canceled (DialogActive): npc = " << npc.playerController->getScriptInstance().name[0] << ", ani = " << ani;
            }
            else
            {
                npc.playerController->getEM().onMessage(sm);
            }
        }
    });

    vm->registerExternalFunction("ai_setwalkmode", [=](Daedalus::DaedalusVM& vm) {
        using EventMessages::MovementMessage;

        int32_t walkmode = vm.popDataValue();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            MovementMessage msg;
            msg.subType = MovementMessage::ST_SetWalkMode;
            msg.walkMode = static_cast<MovementMessage::WalkMode>(walkmode);

            npc.playerController->getEM().onMessage(msg);
        }
    });

    vm->registerExternalFunction("ai_stopprocessinfos", [=](Daedalus::DaedalusVM& vm) {
        // the self argument is the NPC, the player is talking with
        uint32_t self = vm.popVar();
        NpcHandle hself = ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(self).instanceDataHandle);

        // Notify DialogManager
        pWorld->getDialogManager().queueDialogEndEvent(hself);
    });

    vm->registerExternalFunction("ai_output", [=](Daedalus::DaedalusVM& vm) {
        std::string outputname = vm.popString();
        uint32_t target = vm.popVar();
        uint32_t self = vm.popVar();

        NpcHandle hself = ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(self).instanceDataHandle);
        NpcHandle htarget = ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(target).instanceDataHandle);

        auto& dialogManager = pWorld->getDialogManager();
        auto& message = dialogManager.getScriptDialogManager()->getMessageLib().getMessageByName(outputname);
        dialogManager.onAIOutput(hself, htarget, message);
    });

    vm->registerExternalFunction("ai_outputsvm", [=](Daedalus::DaedalusVM& vm) {
        std::string svmname = vm.popString();
        int32_t target = vm.popVar();
        int32_t self = vm.popVar();

        NpcHandle hself = ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(self).instanceDataHandle);
        NpcHandle htarget = ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(target).instanceDataHandle);
        Daedalus::GEngineClasses::C_Npc& cSelf = vm.getGameState().getNpc(hself);

        std::string messageName = "SVM_" + std::to_string(cSelf.voice) + "_" + svmname.substr(1);

        auto& dialogManager = pWorld->getDialogManager();
        auto& messageLib = dialogManager.getScriptDialogManager()->getMessageLib();
        if (!messageLib.messageExists(messageName))
        {
            LogError() << "Error: SVM file not found: " + messageName;  // happens for Character Helper (voice=15) in g2
            return;
        }
        auto& message = messageLib.getMessageByName(messageName);
        dialogManager.onAIOutput(hself, htarget, message);
    });

    vm->registerExternalFunction("AI_ProcessInfos", [=](Daedalus::DaedalusVM& vm) {
        uint32_t self = vm.popVar();

        NpcHandle hself = ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(self).instanceDataHandle);

        auto player = VobTypes::asNpcVob(engine->getMainWorld().get(), engine->getMainWorld().get().getScriptEngine().getPlayerEntity());
        pWorld->getDialogManager().startDialog(hself, player.playerController->getScriptHandle());
    });
}
