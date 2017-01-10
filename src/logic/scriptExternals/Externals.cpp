//
// Created by andre on 03.06.16.
//

#include "Externals.h"
#include <daedalus/DaedalusVM.h>
#include <utils/logger.h>
#include <daedalus/DaedalusStdlib.h>
#include <components/VobClasses.h>
#include <engine/GameEngine.h>
#include <logic/PlayerController.h>
#include <ui/PrintScreenMessages.h>
#include <logic/visuals/ModelVisual.h>
#include <debugdraw/debugdraw.h>

void ::Logic::ScriptExternals::registerStdLib(Daedalus::DaedalusVM& vm, bool verbose)
{
    Daedalus::registerDaedalusStdLib(vm, verbose);
    Daedalus::registerGothicEngineClasses(vm);
}

void ::Logic::ScriptExternals::registerEngineExternals(World::WorldInstance& world, Daedalus::DaedalusVM* vm, bool verbose)
{
    Engine::BaseEngine* engine = world.getEngine();
    World::WorldInstance* pWorld = &world;

    auto isSymInstanceValid = [vm](size_t instance)
    {
        return vm->getDATFile().getSymbolByIndex(instance).instanceDataHandle.isValid();
    };

    // TODO: Refractor
    auto getNPCByInstance = [vm, engine](size_t instance)
    {
		assert(vm->getDATFile().getSymbolByIndex(instance).instanceDataClass == Daedalus::EInstanceClass::IC_Npc);

        Daedalus::GameState::NpcHandle hnpc = ZMemory::handleCast<Daedalus::GameState::NpcHandle>
                (vm->getDATFile().getSymbolByIndex(instance).instanceDataHandle);

        if(!hnpc.isValid())
        {
            LogWarn() << "Invalid handle in instance: " << instance << " (" << vm->getDATFile().getSymbolByIndex(instance).name << ")";

            VobTypes::NpcVobInformation vob;
            vob.entity.invalidate();

            LogInfo() << "Callstack: " << vm->getCallStack();

            return vob;
        }

        // Get data of npc this belongs to
        Daedalus::GEngineClasses::C_Npc& npcData = vm->getGameState().getNpc(hnpc);
        VobTypes::ScriptInstanceUserData* userData = reinterpret_cast<VobTypes::ScriptInstanceUserData*>(npcData.userPtr);

		
		if(userData)
		{
			World::WorldInstance& world = engine->getWorldInstance(userData->world);
			VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(world, userData->vobEntity);

			return vob;
		}
		else{
			LogWarn() << "No userptr on npc: " << npcData.name[0];

			VobTypes::NpcVobInformation vob;
			vob.entity.invalidate();

			return vob;
		}

        
    };

    auto getItemByInstance = [vm, engine](size_t instance)
    {
        Daedalus::GameState::ItemHandle hitem = ZMemory::handleCast<Daedalus::GameState::ItemHandle>
                (vm->getDATFile().getSymbolByIndex(instance).instanceDataHandle);

        // Get data of npc this belongs to
        Daedalus::GEngineClasses::C_Item& itemData = vm->getGameState().getItem(hitem);
        VobTypes::ScriptInstanceUserData* userData = reinterpret_cast<VobTypes::ScriptInstanceUserData*>(itemData.userPtr);

		if(userData)
		{
			World::WorldInstance& world = engine->getWorldInstance(userData->world);
			Vob::VobInformation vob = Vob::asVob(world, userData->vobEntity);

			return vob;
		}
		else{
			LogWarn() << "No userptr on item: " << itemData.name;

			Vob::VobInformation vob;
			vob.entity.invalidate();

			return vob;
		}
    };

    /**
     * Mdl_SetVisual
     */
    vm->registerExternalFunction("Mdl_SetVisual", [=](Daedalus::DaedalusVM& vm) {
        std::string visual = vm.popString();

        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

        // TODO: Need a better API for this
        Daedalus::GameState::NpcHandle hnpc = ZMemory::handleCast<Daedalus::GameState::NpcHandle>
                (vm.getDATFile().getSymbolByIndex(self).instanceDataHandle);
        Daedalus::GEngineClasses::C_Npc& npcData = vm.getGameState().getNpc(hnpc);

        if(npcData.userPtr && vm.getDATFile().getSymbolByIndex(self).instanceDataHandle.isValid())
        {
            VobTypes::ScriptInstanceUserData* userData = reinterpret_cast<VobTypes::ScriptInstanceUserData*>(npcData.userPtr);

            //LogInfo() << "Setting MDL visual to: " << visual;

            World::WorldInstance& world = engine->getWorldInstance(userData->world);
            VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(world, userData->vobEntity);
            VobTypes::NPC_SetModelVisual(vob, visual);
        }
    });

    /**
     * Mdl_SetVisualBody
     */
    vm->registerExternalFunction("Mdl_SetVisualBody", [=](Daedalus::DaedalusVM& vm){

        int32_t armorInstance = vm.popDataValue();
        size_t teethTexNr = static_cast<size_t>(vm.popDataValue());
        size_t headTexNr = static_cast<size_t>(vm.popDataValue());
        std::string head = vm.popString();
        int32_t bodyTexColor = vm.popDataValue();
        int32_t bodyTexNr = vm.popDataValue();
        std::string body = vm.popString();

        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;




        // TODO: Need a better API for this
        Daedalus::GameState::NpcHandle hnpc = ZMemory::handleCast<Daedalus::GameState::NpcHandle>
                (vm.getDATFile().getSymbolByIndex(self).instanceDataHandle);
        Daedalus::GEngineClasses::C_Npc& npcData = vm.getGameState().getNpc(hnpc);

        //LogInfo() << "Self hdl idx:" << vm.getDATFile().getSymbolByIndex(self).instanceDataHandle.index << ", Valid: " << vm.getDATFile().getSymbolByIndex(self).instanceDataHandle.isValid();
        //LogInfo() << " - Userdata: " << npcData.userPtr;

        // TODO: Implement the other visuals
        if(npcData.userPtr && vm.getDATFile().getSymbolByIndex(self).instanceDataHandle.isValid())
        {
            VobTypes::ScriptInstanceUserData* userData = reinterpret_cast<VobTypes::ScriptInstanceUserData*>(npcData.userPtr);

            if(verbose) LogInfo() << "Setting visual to: " << body;

            World::WorldInstance& world = engine->getWorldInstance(userData->world);
            VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(world, userData->vobEntity);

            // Set main visual
            VobTypes::NPC_SetBodyMesh(vob, body, bodyTexNr, bodyTexColor);

            // Set head-mesh
            VobTypes::NPC_SetHeadMesh(vob, head, headTexNr, teethTexNr);
        }

        // TODO: Wrap this up
        // Set armor visual
        if(armorInstance != -1)
        {
            // TODO: Right now, this equips the item automatically. When this is done properly, call the equip-method here
            vm.getGameState().createInventoryItem(static_cast<size_t>(armorInstance), hnpc);
        }
    });

    /**
     * ta_min
     */
    vm->registerExternalFunction("ta_min", [=](Daedalus::DaedalusVM& vm){
        std::string waypoint = vm.popString(); if(verbose) LogInfo() << "waypoint: " << waypoint;

        uint32_t action = vm.popDataValue();
        int32_t stop_m = vm.popDataValue(); if(verbose) LogInfo() << "stop_m: " << stop_m;
        int32_t stop_h = vm.popDataValue(); if(verbose) LogInfo() << "stop_h: " << stop_h;
        int32_t start_m = vm.popDataValue(); if(verbose) LogInfo() << "start_m: " << start_m;
        int32_t start_h = vm.popDataValue(); if(verbose) LogInfo() << "start_h: " << start_h;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            npc.playerController->getAIStateMachine().insertRoutine(start_h, start_m, stop_h, stop_m, action, waypoint);
        }
    });

    /**
     * EquipItem
     */
    vm->registerExternalFunction("equipitem", [=](Daedalus::DaedalusVM& vm){
        uint32_t instance = static_cast<uint32_t>(vm.popDataValue());
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

		if(npc.isValid())
		{
			Daedalus::GameState::ItemHandle item = vm.getGameState().createInventoryItem(instance,
                                                                                         VobTypes::getScriptHandle(npc));
			VobTypes::NPC_EquipWeapon(npc, item);
		}
		else{
			//LogWarn() << "No userptr on npc: " << npcData.name[0];
		}
    });

    /**
     * GetDistTo...
     */
    vm->registerExternalFunction("npc_getdisttonpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc2;
        uint32_t npc2 = vm.popVar(arr_npc2); if(verbose) LogInfo() << "npc2: " << npc2;
        uint32_t arr_npc1;
        uint32_t npc1 = vm.popVar(arr_npc1); if(verbose) LogInfo() << "npc1: " << npc1;

        if(!isSymInstanceValid(npc1) || !isSymInstanceValid(npc2))
            return INT32_MAX;

        VobTypes::NpcVobInformation vob1 = getNPCByInstance(npc1);
        VobTypes::NpcVobInformation vob2 = getNPCByInstance(npc2);

		if(vob1.isValid() && vob2.isValid())
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

    vm->registerExternalFunction("npc_getdisttowp", [=](Daedalus::DaedalusVM& vm){
        std::string wpname = vm.popString(); if(verbose) LogInfo() << "wpname: " << wpname;
        uint32_t arr_self;
        uint32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

        VobTypes::NpcVobInformation vob = getNPCByInstance(self);

        // Calculate distance
        Math::Matrix selfTransform = Vob::getTransform(vob);
        World::WorldInstance& world = Vob::getWorld(vob);
        size_t wpidx = World::Waynet::getWaypointIndex(world.getWaynet(), wpname);

		if(wpidx != World::Waynet::INVALID_WAYPOINT)
		{
			float dist = (selfTransform.Translation() - world.getWaynet().waypoints[wpidx].position).length();

			// Convert to cm
			dist *= 100.0f;

			vm.setReturn(static_cast<int32_t>(dist));
		}
		else {
			vm.setReturn(INT32_MAX);
		}
        
    });

    vm->registerExternalFunction("npc_getdisttoitem", [=](Daedalus::DaedalusVM& vm){

        uint32_t item = static_cast<uint32_t>(vm.popDataValue());
        uint32_t arr_npc;
        int32_t npc = vm.popVar(arr_npc); if(verbose) LogInfo() << "npc: " << npc;

        VobTypes::NpcVobInformation npcvob = getNPCByInstance(npc);
        Vob::VobInformation itemvob = getItemByInstance(item);

        float dist = (Vob::getTransform(npcvob).Translation() - Vob::getTransform(itemvob).Translation()).length();

        // Convert to cm
        dist *= 100.0f;

        vm.setReturn(static_cast<int32_t>(dist));
    });

    vm->registerExternalFunction("npc_getdisttoplayer", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_npc1;
        uint32_t npc1 = vm.popVar(arr_npc1); if(verbose) LogInfo() << "npc1: " << npc1;


        VobTypes::NpcVobInformation vob1 = getNPCByInstance(npc1);
        VobTypes::NpcVobInformation vob2 = getNPCByInstance(vm.getDATFile().getSymbolIndexByName("hero"));

        // Calculate distance
        float dist = (Vob::getTransform(vob1).Translation() - Vob::getTransform(vob2).Translation()).length();

        // Convert to cm
        dist *= 100.0f;

        vm.setReturn(static_cast<int32_t>(dist));
    });

    vm->registerExternalFunction("printdebuginstch", [=](Daedalus::DaedalusVM& vm){

        uint32_t arr_npc;
        std::string s = vm.popString();
        int32_t ch = vm.popDataValue();

        return;

		// Remove some really annoying debug-output
		if(s == "### (0) ### -> B_Scale")
			return;

        LogInfo() << "DEBUG: " << s;
    });

    vm->registerExternalFunction("ai_turntonpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_n1;
        int32_t target = vm.popVar(arr_n1); if(verbose) LogInfo() << "target: " << target;
        uint32_t arr_n0;
        int32_t self = vm.popVar(arr_n0); if(verbose) LogInfo() << "self: " << self;

        if(!isSymInstanceValid(self) || !isSymInstanceValid(target))
            return;

        VobTypes::NpcVobInformation selfvob = getNPCByInstance(self);
        VobTypes::NpcVobInformation targetvob = getNPCByInstance(target);

        // Fill turn message
        EventMessages::MovementMessage msg;
        msg.subType = EventMessages::MovementMessage::ST_TurnToVob;
        msg.targetVob = targetvob.entity;

        // Push the message
        selfvob.playerController->getEM().onMessage(msg);

        vm.setReturn(0);
    });

    /*vm->registerExternalFunction("snd_getdisttosource", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;
        vm.setReturn(0);
    });*/

    vm->registerExternalFunction("printscreen", [=](Daedalus::DaedalusVM& vm){
        int32_t timesec = vm.popDataValue(); if(verbose) LogInfo() << "timesec: " << timesec;
        std::string font = vm.popString(); if(verbose) LogInfo() << "font: " << font;
        int32_t posy = vm.popDataValue(); if(verbose) LogInfo() << "posy: " << posy;
        int32_t posx = vm.popDataValue(); if(verbose) LogInfo() << "posx: " << posx;
        std::string msg = vm.popString(); if(verbose) LogInfo() << "msg: " << msg;
        int32_t dialognr = vm.popDataValue(); if(verbose) LogInfo() << "dialognr: " << dialognr;

        pWorld->getPrintScreenManager().printMessageTimed(posx / 100.0f,
                                                        posy / 100.0f,
                                                        msg,
                                                        static_cast<double>(timesec));
    });

    vm->registerExternalFunction("hlp_getinstanceid", [=](Daedalus::DaedalusVM& vm){
        int32_t sym = vm.popVar();

        // Lookup what's behind this symbol. Could be a reference!
        VobTypes::NpcVobInformation npc = getNPCByInstance(sym);

        if(npc.isValid())
        {
            vm.setReturn((int32_t)npc.playerController->getScriptInstance().instanceSymbol);
        }else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("npc_isplayer", [=](Daedalus::DaedalusVM& vm){
        uint32_t player = vm.popVar(); if(verbose) LogInfo() << "player: " << player;

        VobTypes::NpcVobInformation npc = getNPCByInstance(player);

        if(npc.world && npc.isValid())
        {
            int r = npc.world->getScriptEngine().getPlayerEntity() == npc.entity;
            vm.setReturn(r);
        }else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("npc_canseenpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t other = vm.popVar();
        uint32_t self = vm.popVar();

        if(!isSymInstanceValid(other) || !isSymInstanceValid(self))
        {
            vm.setReturn(0);
            return;
        }

        VobTypes::NpcVobInformation vnpc1 = getNPCByInstance(self);
        VobTypes::NpcVobInformation vnpc2 = getNPCByInstance(other);

        if(vnpc1.isValid() && vnpc2.isValid())
            vm.setReturn(vnpc1.playerController->canSee(vnpc2.entity) ? 1 : 0);
        else
            vm.setReturn(0);

    });

    vm->registerExternalFunction("npc_canseenpcfreelos", [=](Daedalus::DaedalusVM& vm){
        uint32_t other = vm.popVar();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation vnpc1 = getNPCByInstance(self);
        VobTypes::NpcVobInformation vnpc2 = getNPCByInstance(other);

        if(vnpc1.isValid() && vnpc2.isValid())
            vm.setReturn(vnpc1.playerController->canSee(vnpc2.entity, false) ? 1 : 0);
        else
            vm.setReturn(0);
    });

    vm->registerExternalFunction("npc_canseeitem", [=](Daedalus::DaedalusVM& vm){

        uint32_t other = vm.popVar();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation vnpc1 = getNPCByInstance(self);
        Vob::VobInformation vitem = getItemByInstance(other);

        if(vnpc1.isValid() && vitem.isValid())
            vm.setReturn(vnpc1.playerController->canSee(vitem.entity) ? 1 : 0);
        else
            vm.setReturn(0);
    });

    vm->registerExternalFunction("npc_clearaiqueue", [=](Daedalus::DaedalusVM& vm){
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
            npc.playerController->getEM().clear();
    });

    vm->registerExternalFunction("ai_standup", [=](Daedalus::DaedalusVM& vm){
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            // Fill standup message
            EventMessages::MovementMessage msg;
            msg.subType = EventMessages::MovementMessage::ST_Standup;
            msg.targetMode = 1; // Play transition-anis

            // Push the message
            npc.playerController->getEM().onMessage(msg);
        }
    });

    vm->registerExternalFunction("ai_standupquick", [=](Daedalus::DaedalusVM& vm){
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            // Fill standup message
            EventMessages::MovementMessage msg;
            msg.subType = EventMessages::MovementMessage::ST_Standup;
            msg.targetMode = 0; // No transitions

            // Push the message
            npc.playerController->getEM().onMessage(msg);
        }
    });

    vm->registerExternalFunction("npc_exchangeroutine", [=](Daedalus::DaedalusVM& vm){
        std::string routinename = vm.popString(); if(verbose) LogInfo() << "routinename: " << routinename;
        uint32_t arr_self;
        uint32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            npc.playerController->changeRoutine(routinename);
        }
    });

    vm->registerExternalFunction("ai_gotowp", [=](Daedalus::DaedalusVM& vm){
        std::string wp = vm.popString();
        int32_t self = vm.popVar();

        if(!World::Waynet::waypointExists(pWorld->getWaynet(), wp))
            return;

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            EventMessages::MovementMessage sm;
            sm.subType = EventMessages::MovementMessage::ST_GotoVob;
            sm.targetVobName = wp;

            npc.playerController->getEM().onMessage(sm);
            //npc.playerController->gotoWaypoint(World::Waynet::getWaypointIndex(pWorld->getWaynet(), wp));
        }
    });

    vm->registerExternalFunction("ai_gotonextfp", [=](Daedalus::DaedalusVM& vm){
        std::string fp = vm.popString(true);
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            EventMessages::MovementMessage sm;
            sm.subType = EventMessages::MovementMessage::ST_GotoFP;
            sm.targetVobName = fp;
            npc.playerController->getEM().onMessage(sm);
            //npc.playerController->gotoWaypoint(World::Waynet::getWaypointIndex(pWorld->getWaynet(), wp));
        }
    });

    vm->registerExternalFunction("ai_gotonpc", [=](Daedalus::DaedalusVM& vm){
        uint32_t other = vm.popVar();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation nself = getNPCByInstance(self);
        VobTypes::NpcVobInformation nother = getNPCByInstance(other);

        if(nself.isValid())
        {
            EventMessages::MovementMessage sm;
            sm.subType = EventMessages::MovementMessage::ST_GotoVob;
            sm.targetVob = nother.entity;

            nself.playerController->getEM().onMessage(sm);
            //npc.playerController->gotoWaypoint(World::Waynet::getWaypointIndex(pWorld->getWaynet(), wp));
        }
    });

    vm->registerExternalFunction("infomanager_hasfinished", [=](Daedalus::DaedalusVM& vm){
       vm.setReturn(pWorld->getDialogManager().isDialogActive() ? 0 : 1);
    });

    vm->registerExternalFunction("npc_getnearestwp", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            size_t wp = npc.playerController->getClosestWaypoint();

            if(wp != World::Waynet::INVALID_WAYPOINT)
            {
                vm.setReturn(pWorld->getWaynet().waypoints[wp].name);
                return;
            }
        }

        vm.setReturn("");
    });

    vm->registerExternalFunction("npc_getnextwp", [=](Daedalus::DaedalusVM& vm){
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            size_t wp = npc.playerController->getTargetWaypoint();

            if(wp != World::Waynet::INVALID_WAYPOINT)
            {
                vm.setReturn(pWorld->getWaynet().waypoints[wp].name);
                return;
            }
        }

        vm.setReturn("");
    });

	vm->registerExternalFunction("npc_hasitems", [=](Daedalus::DaedalusVM& vm){
		uint32_t iteminstance = vm.popDataValue();
		int32_t owner = vm.popVar();

		VobTypes::NpcVobInformation npc = getNPCByInstance(owner);

		if(npc.isValid())
		{
			int count = npc.playerController->getInventory().getItemCount(iteminstance);
			vm.setReturn(count);
		}
		else
		{
			vm.setReturn(0);
		}
	});

	vm->registerExternalFunction("npc_removeinvitem", [=](Daedalus::DaedalusVM& vm){
		uint32_t iteminstance = vm.popDataValue(); 
		uint32_t owner = vm.popVar(); 

		VobTypes::NpcVobInformation npc = getNPCByInstance(owner);

		if(npc.isValid())
		{
			int count = npc.playerController->getInventory().removeItem(iteminstance);
		}

		vm.setReturn(0);
	});

	vm->registerExternalFunction("npc_removeinvitems", [=](Daedalus::DaedalusVM& vm){
		uint32_t amount = vm.popDataValue();
		uint32_t iteminstance = vm.popDataValue();
		uint32_t owner = vm.popVar();

		VobTypes::NpcVobInformation npc = getNPCByInstance(owner);

		if(npc.isValid())
		{
			int count = npc.playerController->getInventory().removeItem(iteminstance, amount);
		}

		vm.setReturn(0);
	});

    vm->registerExternalFunction("ai_startstate", [=](Daedalus::DaedalusVM& vm){
        std::string wpname = vm.popString();
        int32_t statebehaviour = vm.popDataValue();
        uint32_t fnSym = vm.popVar();
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            EventMessages::StateMessage sm;
            sm.subType = EventMessages::StateMessage::EV_StartState;
            sm.wpname = wpname;
            sm.functionSymbol = fnSym;

            npc.playerController->getEM().onMessage(sm);
        }
    });

    vm->registerExternalFunction("npc_getstatetime", [=](Daedalus::DaedalusVM& vm){
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            vm.setReturn((int)npc.playerController->getAIStateMachine().getCurrentStateTime());
        }else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("wld_detectnpc", [=](Daedalus::DaedalusVM& vm){
        int32_t guild = vm.popDataValue();
        int32_t aiState = vm.popDataValue();
        int32_t instance = vm.popDataValue();
        uint32_t self = vm.popVar();

        // TODO: Find using guild, etc

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            const std::set<Handle::EntityHandle>& worldNPCs = pWorld->getScriptEngine().getWorldNPCs();

            // Find the nearest NPC with the given criteria
            Math::float3 center = npc.position->m_WorldMatrix.Translation();

            //ddDrawAxis(center.x, center.y + 2, center.z, 2.0f);

            float nearest = FLT_MAX;
            Handle::EntityHandle nearestEnt;
            for(Handle::EntityHandle e : worldNPCs)
            {
                // Check position first (faster)
                Math::float3 position = pWorld->getEntity<Components::PositionComponent>(e).m_WorldMatrix.Translation();

                float dist = (center - position).lengthSquared();
                if(dist < nearest)
                {
                    if(e == npc.entity)
                        continue;

                    VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(*pWorld, e);
                    Daedalus::GEngineClasses::C_Npc& scriptInstance = VobTypes::getScriptObject(vob);

                    if(instance >= 0 && scriptInstance.instanceSymbol != instance) continue;
                    if(guild >= 0 && scriptInstance.guild != guild) continue;
                    if(aiState >= 0 && vob.playerController->getAIStateMachine().isInState((size_t)aiState)) continue;

                    nearestEnt = e;
                    nearest = dist;
                }
            }

            // If found, put it into other
            if(nearestEnt.isValid())
            {
                VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(*pWorld, nearestEnt);

                pWorld->getScriptEngine().setInstanceNPC("other", VobTypes::getScriptHandle(vob));
            }

            vm.setReturn(nearestEnt.isValid() ? 1 : 0);
        }else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("wld_istime", [=](Daedalus::DaedalusVM& vm){

        int32_t min2 = vm.popDataValue();
        int32_t hour2 = vm.popDataValue();
        int32_t min1 = vm.popDataValue();
        int32_t hour1 = vm.popDataValue();

        int32_t hour, min;
        pWorld->getSky().getTimeOfDay(hour, min);

        if (hour >= hour1 && hour <= hour2 &&
            min >= min1 && min >= min2)
        {
            vm.setReturn(1);
        } else
            vm.setReturn(0);
    });

    vm->registerExternalFunction("ai_wait", [=](Daedalus::DaedalusVM& vm){
        float duration = vm.popFloatValue();
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            EventMessages::StateMessage sm;
            sm.subType = EventMessages::StateMessage::EV_Wait;
            sm.waitTime = duration;

            npc.playerController->getEM().onMessage(sm);
        }

        vm.setReturn(0);
    });

    vm->registerExternalFunction("ai_playani", [=](Daedalus::DaedalusVM& vm){
        std::string ani = vm.popString();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            EventMessages::ConversationMessage sm;
            sm.subType = EventMessages::ConversationMessage::ST_PlayAni;
            sm.animation = ani;

            npc.playerController->getEM().onMessage(sm);
        }

        vm.setReturn(0);
    });

    vm->registerExternalFunction("mdl_applyoverlaymds", [=](Daedalus::DaedalusVM& vm){
        std::string overlayname = vm.popString();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            npc.playerController->getModelVisual()->applyOverlay(overlayname);
        }

        vm.setReturn(0);
    });

    vm->registerExternalFunction("mdl_removeoverlaymds", [=](Daedalus::DaedalusVM& vm){
        std::string overlayname = vm.popString();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            // TODO: Implement using of multiple overlays!
            npc.playerController->getModelVisual()->applyOverlay("");
        }

        vm.setReturn(0);
    });

    vm->registerExternalFunction("wld_isfpavailable", [=](Daedalus::DaedalusVM& vm){
        std::string fpname = vm.popString(true);
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            // Find closest fp
            std::vector<Handle::EntityHandle> fp = pWorld->getFreepointsInRange(npc.position->m_WorldMatrix.Translation(), 100.0f, fpname, true);

            vm.setReturn(!fp.empty());
        }else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("wld_isnextfpavailable", [=](Daedalus::DaedalusVM& vm){
        std::string fpname = vm.popString(true);
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            // Find closest fp
            std::vector<Handle::EntityHandle> fp = pWorld->getFreepointsInRange(npc.position->m_WorldMatrix.Translation(), 100.0f, fpname, true, npc.entity);

            vm.setReturn(!fp.empty());
        }else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("npc_isdead", [=](Daedalus::DaedalusVM& vm){
        int32_t n = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(n);

        if(npc.isValid())
        {
            vm.setReturn(npc.playerController->getBodyState() == BS_DEAD ? 1 : 0);
        }else
        {
            vm.setReturn(1); // Default to "Yes, he's dead" in case he wasn't even loaded
        }
    });

    vm->registerExternalFunction("npc_isonfp", [=](Daedalus::DaedalusVM& vm){
        std::string fpname = vm.popString(true);
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            // Find fp with given name
            // TODO
            /*Handle::EntityHandle fp = (*pWorld->getFreepoints().find(fpname)).second;
            Components::SpotComponent& sp = pWorld->getEntity<Components::SpotComponent>(fp);

            vm.setReturn(sp.m_UsingEntity == npc.entity && sp.m_UseEndTime < pWorld->getWorldInfo().time);*/
        }else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("info_addchoice", [=](Daedalus::DaedalusVM& vm){
        uint32_t func = vm.popVar();
        std::string text = vm.popString();
        uint32_t info = vm.popVar();

        Daedalus::GameState::InfoHandle hinfo = ZMemory::handleCast<Daedalus::GameState::InfoHandle>(
                pWorld->getScriptEngine().getVM().getDATFile().getSymbolByIndex(info).instanceDataHandle);

        Logic::DialogManager::ChoiceEntry choice;
        choice.info = hinfo;
        choice.text = text;
        choice.nr = -2; // This means: Ascending order
        choice.functionSym = func;

        pWorld->getDialogManager().addChoice(choice);
    });

    vm->registerExternalFunction("info_clearchoices", [=](Daedalus::DaedalusVM& vm){
        uint32_t info = vm.popVar();

        pWorld->getDialogManager().clearChoices();
    });

	vm->registerExternalFunction("wld_insertnpc", [=](Daedalus::DaedalusVM& vm){
		std::string spawnpoint = vm.popString(); 
		uint32_t npcinstance = vm.popDataValue();

		if(spawnpoint != "" && !World::Waynet::waypointExists(pWorld->getWaynet(), spawnpoint))
		{
			LogWarn() << "Invalid location: " << spawnpoint;
			return;
		}

		vm.getGameState().insertNPC(npcinstance, spawnpoint);
	});

    vm->registerExternalFunction("wld_insertitem", [=](Daedalus::DaedalusVM& vm){
        std::string spawnpoint = vm.popString(true);
        uint32_t iteminstance = vm.popDataValue();

        // Create object
        Handle::EntityHandle e = VobTypes::initItemFromScript(*pWorld, iteminstance);

        // Position the vob
        Vob::VobInformation vob = Vob::asVob(*pWorld, e);

        // Try vobs first
        Handle::EntityHandle spawnEnt = pWorld->getVobEntityByName(spawnpoint);

        Math::float3 position;
        if(spawnEnt.isValid())
        {
            // Get position from object
            Components::PositionComponent& pos = pWorld->getEntity<Components::PositionComponent>(spawnEnt);
            position = pos.m_WorldMatrix.Translation();
        }else
        {
            // Then try waypoints
            World::Waynet::WaypointIndex wp = World::Waynet::getWaypointIndex(pWorld->getWaynet(), spawnpoint);

            if(wp == World::Waynet::INVALID_WAYPOINT)
            {
                LogWarn() << "Invalid location: " << spawnpoint;
                return;
            }

            position = pWorld->getWaynet().waypoints[wp].position;
        }

        // Move item to right place
        Vob::setPosition(vob, position);
    });

    vm->registerExternalFunction("npc_changeattribute", [=](Daedalus::DaedalusVM& vm){
        int32_t value = vm.popDataValue();
        int32_t atr = vm.popDataValue();
        uint32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if(npc.isValid())
        {
            npc.playerController->changeAttribute((Daedalus::GEngineClasses::C_Npc::EAttributes)atr, value);
        }
    });

    vm->registerExternalFunction("npc_giveitem", [=](Daedalus::DaedalusVM& vm){
        if(verbose) LogInfo() << "npc_giveitem";

        uint32_t fromNpcId = vm.popVar(); if(verbose) LogInfo() << "from" << fromNpcId;
        uint32_t itemInstance = vm.popVar(); if(verbose) LogInfo() << "item" << itemInstance;
        uint32_t toNpcId = vm.popVar(); if(verbose) LogInfo() << "to" << toNpcId;

        VobTypes::NpcVobInformation fromNpc = getNPCByInstance(fromNpcId);
        VobTypes::NpcVobInformation toNpc = getNPCByInstance(toNpcId);

        if(!fromNpc.isValid() || !toNpc.isValid())
            return;
        if(!fromNpc.playerController->getInventory().removeItem(itemInstance))
            return;

        toNpc.playerController->getInventory().addItem(itemInstance);
    });

    vm->registerExternalFunction("snd_play", [=](Daedalus::DaedalusVM& vm) {
        if(verbose) LogInfo() << "snd_play";
        std::string s0 = vm.popString(); if(verbose) LogInfo() << "s0: " << s0;

        pWorld->getAudioEngine().playSound(s0 + ".wav");
    });
}






