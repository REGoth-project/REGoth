//
// Created by desktop on 12.02.17.
//

#include "NetScriptEngine.h"
#include <engine/World.h>
#include <logic/ScriptEngine.h>
#include <daedalus/DaedalusVM.h>
#include <ZenLib/utils/logger.h>
#include <SFNUL.hpp>
#include <engine/NetEngine.h>
#include <logic/SyncedObjects.h>
#include <logic/ServerState.h>

namespace sfn
{
    sfn::Message& operator<<(sfn::Message& message, const ZMemory::BigHandle& input)
    {
        message << (uint32_t)input.index;
        message << (uint32_t)input.generation;

        return message;
    }

    sfn::Message& operator>>(sfn::Message& message, ZMemory::BigHandle& output)
    {
        uint32_t index, generation;
        message >> index;
        message >> generation;

        output.index = index;
        output.generation = generation;

        return message;
    }

    sfn::Message& operator<<(sfn::Message& message, const Daedalus::PARSymbol& input)
    {
        message << input.address;
        message << input.floatData;
        message << input.strData;
        message << input.intData;

        return message;
    }

    sfn::Message& operator>>(sfn::Message& message, Daedalus::PARSymbol& output)
    {
        message >> output.address;
        message >> output.floatData;
        message >> output.strData;
        message >> output.intData;

        return message;
    }

    sfn::Message& operator<<(sfn::Message& message, const Daedalus::GEngineClasses::C_Npc& input)
    {
        message << input.id;
        message << input.name;
        message << input.slot;
        message << input.npcType;
        message << input.flags;
        message << input.attribute;
        message << input.protection;
        message << input.damage;
        message << input.damagetype;
        message << input.guild;
        message << input.level;
        message << input.mission;
        message << input.fight_tactic;
        message << input.weapon;
        message << input.voice;
        message << input.voicePitch;
        message << input.bodymass;
        message << input.daily_routine;
        message << input.start_aistate;
        message << input.spawnPoint;
        message << input.spawnDelay;
        message << input.senses;
        message << input.senses_range;
        message << input.ai;
        message << input.wp;
        message << input.exp;
        message << input.exp_next;
        message << input.lp;

        return message;
    }

    sfn::Message& operator>>(sfn::Message& message, Daedalus::GEngineClasses::C_Npc& output)
    {
        message >> output.id;
        message >> output.name;
        message >> output.slot;
        message >> output.npcType;
        message >> output.flags;
        message >> output.attribute;
        message >> output.protection;
        message >> output.damage;
        message >> output.damagetype;
        message >> output.guild;
        message >> output.level;
        message >> output.mission;
        message >> output.fight_tactic;
        message >> output.weapon;
        message >> output.voice;
        message >> output.voicePitch;
        message >> output.bodymass;
        message >> output.daily_routine;
        message >> output.start_aistate;
        message >> output.spawnPoint;
        message >> output.spawnDelay;
        message >> output.senses;
        message >> output.senses_range;
        message >> output.ai;
        message >> output.wp;
        message >> output.exp;
        message >> output.exp_next;
        message >> output.lp;

        return message;
    }
}

Net::NetScriptEngine::NetScriptEngine(Engine::NetEngine& engine, World::WorldInstance& world) :
    m_World(world),
    m_Engine(engine)
{

}

Net::NetScriptEngine::~NetScriptEngine()
{

}

void Net::NetScriptEngine::registerHooks()
{
    Daedalus::DaedalusVM& vm = m_World.getScriptEngine().getVM();

    // We need to know when symbols get changed
    vm.setOnSymbolValueChangedCallback([this](unsigned sym, Daedalus::EParOp op){
        onSymbolChanged(sym, op);
    });

    // We need to know when an instance got created so the clients can map their instances to the servers
    vm.getGameState().setOnInstanceCreatedCallback([this](ZMemory::BigHandle handle, Daedalus::EInstanceClass instanceClass){
        onInstanceCreated(handle, instanceClass);
    });

    vm.getGameState().setOnInstanceRemovedCallback([this](ZMemory::BigHandle handle, Daedalus::EInstanceClass instanceClass){
        onInstanceRemoved(handle, instanceClass);
    });
}

void Net::NetScriptEngine::onSymbolChanged(unsigned sym, Daedalus::EParOp op)
{
    Daedalus::DaedalusVM& vm = m_World.getScriptEngine().getVM();
    sfn::Message msg;
    switch(op)
    {
        // Integer ops
        case Daedalus::EParOp_Assign:
        case Daedalus::EParOp_AssignAdd:
        case Daedalus::EParOp_AssignSubtract:
        case Daedalus::EParOp_AssignMultiply:
        case Daedalus::EParOp_AssignDivide:
        case Daedalus::EParOp_AssignString:
        case Daedalus::EParOp_AssignFunc:
        case Daedalus::EParOp_AssignFloat:
        {
            auto& s = vm.getDATFile().getSymbolByIndex(sym);

            if(s.name.find("DEBUG") != std::string::npos)
                return; // Skip everything debug-related

            // Check whether this is just plain data or whether it's a full blown instance
            if(!vm.getCurrentInstanceDataPtr())
            {
                LogInfo() << "Net: Script symbol changed (Plain data): " << s.name;

                // Plain data
                msg << ScriptPacket::SP_SymChanged;
                msg << (uint32_t)sym;
                msg << s;
            }else
            {


                // Filter instances we are interested in
                switch(vm.getCurrentInstanceClass())
                {
                    case Daedalus::IC_Npc:

                        //LogInfo() << "Net: NPC-Script symbol changed (Instance-data): " << s.name;

                        msg << ScriptPacket::SP_InstanceChanged;
                        msg << vm.getCurrentInstanceClass();
                        msg << vm.getCurrentInstanceHandle();
                        msg << vm.getGameState().getNpc(ZMemory::handleCast<Daedalus::GameState::NpcHandle>(vm.getCurrentInstanceHandle()));
                        break;

                    // Uninteresting
                    case Daedalus::IC_Mission:
                    case Daedalus::IC_Info:
                    case Daedalus::IC_Item:
                    case Daedalus::IC_ItemReact:
                    case Daedalus::IC_Focus:
                    case Daedalus::IC_Menu:
                    case Daedalus::IC_MenuItem:
                    case Daedalus::IC_Sfx:
                    case Daedalus::IC_Pfx:
                    default:
                        ;
                }
            }
        }
            break;

        case Daedalus::EParOp_AssignInstance:
            msg << ScriptPacket::SP_SymInstanceChanged;
            msg << (uint32_t)sym;
            msg << vm.getCurrentInstanceClass();
            msg << vm.getCurrentInstanceHandle();
            break;
        default:
            LogWarn() << "Net: Symbol changed by non-assign operation!";
    }

    if(msg.GetSize())
        getServerState().broadcast(StreamID::ScriptStream, msg);
}

Net::ServerState& Net::NetScriptEngine::getServerState()
{
    return *m_Engine.getServerState();
}

void Net::NetScriptEngine::onInstanceCreated(ZMemory::BigHandle handle, Daedalus::EInstanceClass instanceClass)
{
    /*sfn::Message msg;

    LogInfo() << "Net: Instance created: " << handle.index << " (class " << instanceClass << ")";

    msg << ScriptPacket::SP_InstanceCreated;
    msg << instanceClass;
    msg << handle;

    getServerState().broadcast(StreamID::ScriptStream, msg);*/
}

void Net::NetScriptEngine::onInstanceRemoved(ZMemory::BigHandle handle, Daedalus::EInstanceClass instanceClass)
{
    /*sfn::Message msg;

    LogInfo() << "Net: Instance removed: " << handle.index << " (class " << instanceClass << ")";

    msg << ScriptPacket::SP_InstanceRemoved;
    msg << instanceClass;
    msg << handle;

    getServerState().broadcast(StreamID::ScriptStream, msg);*/
}
