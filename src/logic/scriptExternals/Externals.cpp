//
// Created by andre on 03.06.16.
//

#include "Externals.h"
#include <daedalus/DaedalusVM.h>
#include <utils/logger.h>
#include <daedalus/DaedalusStdlib.h>
#include <components/VobClasses.h>

void ::Logic::ScriptExternals::registerStdLib(Daedalus::DaedalusVM& vm, bool verbose)
{
    Daedalus::registerDaedalusStdLib(vm, verbose);
    Daedalus::registerGothicEngineClasses(vm);
}

void ::Logic::ScriptExternals::registerEngineExternals(Daedalus::DaedalusVM& vm, bool verbose)
{
    int		bodyTexNr,bodyTexColor,headTexNr,teethTexNr,armorInstance;
    vm.registerExternalFunction("Mdl_SetVisualBody", [=](Daedalus::DaedalusVM& vm){

        int32_t armorInstance = vm.popDataValue();
        int32_t teethInstance = vm.popDataValue();
        int32_t headTexNr = vm.popDataValue();
        std::string head = vm.popString();
        int32_t bodyTexColor = vm.popDataValue();
        int32_t bodyTexNr = vm.popDataValue();
        std::string body = vm.popString();

        uint32_t arr_self;
        int32_t self = vm.popVar(arr_self); if(verbose) LogInfo() << "self: " << self;

        // TODO: Implement the other visuals
        // TODO: Need a better API for this
        Daedalus::GameState::NpcHandle hnpc = ZMemory::handleCast<Daedalus::GameState::NpcHandle>
                (vm.getDATFile().getSymbolByIndex(self).instanceDataHandle);
        Daedalus::GEngineClasses::C_Npc& npcData = vm.getGameState().getNpc(hnpc);

        if(npcData.userPtr)
        {
            VobTypes::ScriptInstanceUserData* userData = reinterpret_cast<VobTypes::ScriptInstanceUserData*>(npcData.userPtr);

            //userData->vobEntity
        }
    });
}






