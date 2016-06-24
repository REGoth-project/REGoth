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

void ::Logic::ScriptExternals::registerStdLib(Daedalus::DaedalusVM& vm, bool verbose)
{
    Daedalus::registerDaedalusStdLib(vm, verbose);
    Daedalus::registerGothicEngineClasses(vm);
}

void ::Logic::ScriptExternals::registerEngineExternals(Engine::BaseEngine* engine, Daedalus::DaedalusVM* vm, bool verbose)
{
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
            Vob::setVisual(vob, body + ".MDM");

            // Set head-mesh
            VobTypes::NPC_SetHeadMesh(vob, head, headTexNr, teethTexNr);
        }

        // TODO: Wrap this up
        // Set armor visual
        if(armorInstance != -1)
        {
            // TODO: Right now, this equips the item automatically. When this is done properly, call the equip-method here
            vm.getGameState().addInventoryItem(static_cast<size_t>(armorInstance), hnpc);
        }
    });

    vm->registerExternalFunction("ta_min", [=](Daedalus::DaedalusVM& vm){
        std::string waypoint = vm.popString(); if(verbose) LogInfo() << "waypoint: " << waypoint;

        int32_t action = vm.popDataValue();
        int32_t stop_m = vm.popDataValue(); if(verbose) LogInfo() << "stop_m: " << stop_m;
        int32_t stop_h = vm.popDataValue(); if(verbose) LogInfo() << "stop_h: " << stop_h;
        int32_t start_m = vm.popDataValue(); if(verbose) LogInfo() << "start_m: " << start_m;
        int32_t start_h = vm.popDataValue(); if(verbose) LogInfo() << "start_h: " << start_h;
        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

        // TODO: Need a better API for this
        Daedalus::GameState::NpcHandle hnpc = ZMemory::handleCast<Daedalus::GameState::NpcHandle>
                (vm.getDATFile().getSymbolByIndex(self).instanceDataHandle);
        Daedalus::GEngineClasses::C_Npc& npcData = vm.getGameState().getNpc(hnpc);

        if(verbose) LogInfo() << "Self: " << self << ", " << vm.getDATFile().getSymbolByIndex(self).name << "  ...  hdl idx:" << vm.getDATFile().getSymbolByIndex(self).instanceDataHandle.index << ", Valid: " << vm.getDATFile().getSymbolByIndex(self).instanceDataHandle.isValid();
        if(verbose) LogInfo() << " - Userdata: " << npcData.userPtr;

        if(npcData.userPtr && vm.getDATFile().getSymbolByIndex(self).instanceDataHandle.isValid())
        {
            VobTypes::ScriptInstanceUserData* userData = reinterpret_cast<VobTypes::ScriptInstanceUserData*>(npcData.userPtr);

            World::WorldInstance& world = engine->getWorldInstance(userData->world);
            VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(world, userData->vobEntity);

            if(verbose) LogInfo() << "NPC " << npcData.name[0] << " going to wp: " << waypoint;

            if(World::Waynet::waypointExists(world.getWaynet(), waypoint))
            {
                vob.playerController->teleportToWaypoint(World::Waynet::getWaypointIndex(world.getWaynet(), waypoint));
                vob.playerController->addRoutineWaypoint(World::Waynet::getWaypointIndex(world.getWaynet(), waypoint));
            }
        }
    });
}






