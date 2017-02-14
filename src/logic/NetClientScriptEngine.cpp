//
// Created by desktop on 12.02.17.
//

#include <ZenLib/daedalus/DaedalusVM.h>
#include "NetClientScriptEngine.h"
#include <engine/World.h>
#include <logic/ScriptEngine.h>
#include <ZenLib/utils/logger.h>
#include <engine/NetEngine.h>
#include <logic/ClientState.h>
#include <components/VobClasses.h>
#include <logic/PlayerController.h>
#include <logic/ItemController.h>

const bool VERBOSE_LOGGING = false;

Net::NetClientScriptEngine::NetClientScriptEngine(Engine::NetEngine& engine, World::WorldInstance& world) :
        m_World(world),
        m_Engine(engine)
{

}

Net::NetClientScriptEngine::~NetClientScriptEngine()
{

}

void Net::NetClientScriptEngine::readStreams()
{
    sfn::Message msg;
    while(getClientState().m_Link->Receive(StreamID::ScriptStream, msg))
    {
        if(!getClientState().isConnected())
            continue;

        ScriptPacket p;
        msg >> p;

        switch(p)
        {
            case SP_SymChanged:
            {
                uint32_t sym;
                Daedalus::EParOp op;
                Daedalus::PARSymbol newSym;

                msg >> sym;
                msg >> op;
                msg >> newSym;

                onSymbolChanged(sym, op, newSym);
            }
                break;

            case SP_InstanceChanged:
            {
                ZMemory::BigHandle serverHandle;
                Daedalus::EInstanceClass instanceClass;
                msg >> instanceClass;
                msg >> serverHandle;

                onInstanceDataChanged(serverHandle, instanceClass, msg);
            }
                break;

            case SP_SymInstanceChanged:
            {
                uint32_t sym;
                ZMemory::BigHandle serverHandle;
                Daedalus::EInstanceClass instanceClass;

                msg >> sym;
                msg >> instanceClass;
                msg >> serverHandle;

                onSymbolInstanceAssigned(sym, serverHandle, instanceClass);
            }
                break;

            case SP_InstanceCreated:
            {
                ZMemory::BigHandle serverHandle;
                Daedalus::EInstanceClass instanceClass;

                msg >> instanceClass;
                msg >> serverHandle;

                onInstanceCreated(serverHandle, instanceClass);
            }
                break;
            case SP_InstanceRemoved:
            {
                ZMemory::BigHandle serverHandle;
                Daedalus::EInstanceClass instanceClass;

                msg >> instanceClass;
                msg >> serverHandle;

                onInstanceRemoved(serverHandle, instanceClass);
            }
                break;

            case SP_NPC_Created:
            {
                uint32_t sym;
                ZMemory::BigHandle serverhandle;
                std::string startpoint;

                msg >> sym;
                msg >> serverhandle;
                msg >> startpoint;

                onNPCCreated(sym, serverhandle, startpoint);
            }
                break;

            case SP_NPC_Teleport:
            {
                ZMemory::BigHandle serverhandle;
                Math::float3 newPosition;
                Math::float3 newDirection;

                msg >> serverhandle;
                msg >> newPosition;
                msg >> newDirection;

                onNPCTeleport(serverhandle, newPosition, newDirection);
            }
                break;

            case SP_NPC_Killed:
            {
                ZMemory::BigHandle killed;
                ZMemory::BigHandle killer;

                msg >> killed;
                msg >> killer;

                onNPCKilled(killed, killer);
            }
                break;

            case SP_WorldTimeSync:
            {
                float time;
                msg >> time;

                m_World.getSky().setTimeOfDay(time);
            }
            break;

            case SP_Item_Created:
            {
                Handle::EntityHandle serverhandle;
                uint32_t sym;
                Math::Matrix transform;

                msg >> serverhandle;
                msg >> sym;
                msg >> transform;

                onItemInserted(serverhandle, sym, transform);
            }
            break;

            case SP_Item_Removed:
            {
                Handle::EntityHandle serverhandle;
                msg >> serverhandle;

                onItemTaken(serverhandle);
            }
            break;

            case SP_NPC_AddInventory:
            {
                ZMemory::BigHandle serverhandle;
                uint32_t sym;
                int32_t count;

                msg >> serverhandle;
                msg >> sym;
                msg >> count;

                onNPCAddInventory(serverhandle, sym, count);
            }break;
            default:
                LogWarn() << "Net: Unknown script-packet: " << p;
        }
    }
}

Net::ClientState& Net::NetClientScriptEngine::getClientState()
{
    return *m_Engine.getClientState();
}


void Net::NetClientScriptEngine::onInstanceCreated(ZMemory::BigHandle serverHandle, Daedalus::EInstanceClass instanceClass)
{
    LogInfo() << "Net: Server created instance of class " << instanceClass << ": " << serverHandle.index;

    Daedalus::DaedalusVM& vm = m_World.getScriptEngine().getVM();
    ZMemory::BigHandle localHandle;
    switch(instanceClass)
    {
        case Daedalus::IC_Npc: localHandle =        ZMemory::toBigHandle(vm.getGameState().createNPC()); break;
        case Daedalus::IC_Mission: localHandle =    ZMemory::toBigHandle(vm.getGameState().createMission()); break;
        case Daedalus::IC_Info: localHandle =       ZMemory::toBigHandle(vm.getGameState().createInfo()); break;
        case Daedalus::IC_Item: localHandle =       ZMemory::toBigHandle(vm.getGameState().createItem()); break;
        case Daedalus::IC_ItemReact: localHandle =  ZMemory::toBigHandle(vm.getGameState().createItemReact()); break;
        case Daedalus::IC_Focus: localHandle =      ZMemory::toBigHandle(vm.getGameState().createFocus()); break;
        case Daedalus::IC_Menu: localHandle =       ZMemory::toBigHandle(vm.getGameState().createMenu()); break;
        case Daedalus::IC_MenuItem: localHandle =   ZMemory::toBigHandle(vm.getGameState().createMenuItem()); break;
        case Daedalus::IC_Sfx: localHandle =        ZMemory::toBigHandle(vm.getGameState().createSfx()); break;
        case Daedalus::IC_Pfx: localHandle =        ZMemory::toBigHandle(vm.getGameState().createPfx()); break;

        default:
            LogInfo() << "Net: Server created unknown script-instance. Class: " << instanceClass;
    }

    registerLocalHandle(instanceClass, serverHandle, localHandle);
}

void Net::NetClientScriptEngine::onInstanceRemoved(ZMemory::BigHandle serverHandle, Daedalus::EInstanceClass instanceClass)
{
    LogInfo() << "Net: Server removed instance of class " << instanceClass << ": " << serverHandle.index;

    using namespace Daedalus::GameState;

    Daedalus::DaedalusVM& vm = m_World.getScriptEngine().getVM();
    ZMemory::BigHandle localHandle = getLocalHandle(instanceClass, serverHandle);

    switch(instanceClass)
    {
        case Daedalus::IC_Item:      vm.getGameState().removeItem(ZMemory::handleCast<ItemHandle>(localHandle));
        case Daedalus::IC_Menu:      vm.getGameState().removeMenu(ZMemory::handleCast<MenuHandle>(localHandle));
        case Daedalus::IC_MenuItem:  vm.getGameState().removeMenuItem(ZMemory::handleCast<MenuItemHandle>(localHandle));

        default:
            LogInfo() << "Net: Server removed unknown/unimplemented script-instance. Class: " << instanceClass;
    }

    // Invalidate handle
    registerLocalHandle(instanceClass, serverHandle, ZMemory::BigHandle());
}

ZMemory::BigHandle Net::NetClientScriptEngine::getServerHandle(Daedalus::EInstanceClass instanceClass,
                                                               ZMemory::BigHandle localHandle)
{
    const auto& instMap = m_InstanceMapLocal[instanceClass];

    const auto it = instMap.find(localHandle);

    if(it == instMap.end())
        return ZMemory::BigHandle(); // Not found, return invalid handle

    return it->second;
}

ZMemory::BigHandle Net::NetClientScriptEngine::getLocalHandle(Daedalus::EInstanceClass instanceClass,
                                                              ZMemory::BigHandle serverHandle)
{
    const auto& instMap = m_InstanceMap[instanceClass];

    const auto it = instMap.find(serverHandle);

    if(it == instMap.end())
        return ZMemory::BigHandle(); // Not found, return invalid handle

    return it->second;
}

void
Net::NetClientScriptEngine::registerLocalHandle(Daedalus::EInstanceClass instanceClass, ZMemory::BigHandle serverHandle,
                                                ZMemory::BigHandle localHandle)
{
    auto& instMap = m_InstanceMap[instanceClass];
    auto& instMapLocal = m_InstanceMapLocal[instanceClass];

    instMap[serverHandle] = localHandle;
    instMapLocal[localHandle] = serverHandle;
}

Handle::EntityHandle Net::NetClientScriptEngine::getLocalEntity(Net::EntityType type, Handle::EntityHandle localHandle)
{
    const auto& instMap = m_EntityMap[type];

    const auto it = instMap.find(localHandle);

    if(it == instMap.end())
        return Handle::EntityHandle::makeInvalidHandle(); // Not found, return invalid handle

    return it->second;
}

Handle::EntityHandle Net::NetClientScriptEngine::getServerEntity(Net::EntityType type, Handle::EntityHandle serverhandle)
{
    const auto& instMap = m_EntityMapLocal[type];

    const auto it = instMap.find(serverhandle);

    if(it == instMap.end())
        return Handle::EntityHandle::makeInvalidHandle(); // Not found, return invalid handle

    return it->second;
}

void Net::NetClientScriptEngine::registerLocalHandle(Net::EntityType type, Handle::EntityHandle serverHandle,
                                                     Handle::EntityHandle localHandle)
{
    auto& instMap = m_EntityMap[type];
    auto& instMapLocal = m_EntityMapLocal[type];

    instMap[serverHandle] = localHandle;
    instMapLocal[localHandle] = serverHandle;
}


void Net::NetClientScriptEngine::onSymbolChanged(unsigned sym, Daedalus::EParOp op, const Daedalus::PARSymbol& newSym)
{
    Daedalus::DaedalusVM& vm = m_World.getScriptEngine().getVM();

    if(VERBOSE_LOGGING) LogInfo() << "Net: Script symbol " << vm.getDATFile().getSymbolByIndex(sym).name << " changed! (Plain data)";

    vm.getDATFile().getSymbolByIndex(sym) = newSym;
}

void Net::NetClientScriptEngine::onInstanceDataChanged(ZMemory::BigHandle serverHandle,
                                                       Daedalus::EInstanceClass instanceClass,
                                                       sfn::Message& data)
{
    using namespace Daedalus::GameState;

    if(VERBOSE_LOGGING) LogInfo() << "Net: Server changed instance of class " << instanceClass << ": " << serverHandle.index;

    Daedalus::DaedalusVM& vm = m_World.getScriptEngine().getVM();

    ZMemory::BigHandle localHandle;
    switch(instanceClass)
    {
        case Daedalus::IC_Npc:
            if(VERBOSE_LOGGING) LogInfo() << "Net: Server changed NPC instance!";
            localHandle = getLocalHandle(instanceClass, serverHandle);

            if(localHandle.isValid())
            {
                data >> vm.getGameState().getNpc(ZMemory::handleCast<NpcHandle>(localHandle));

                // TODO: Figure out if important data-points which need special handling changed
            } else
            {
                if(VERBOSE_LOGGING) LogWarn() << "Net: Server changed NPC-instance unknown to the client! Serverhandle: " << serverHandle.index;
            }
            break;

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
            if(VERBOSE_LOGGING) LogInfo() << "Net: Server changed uninteresting/unimplemented instance of class " << instanceClass;
    }
}

void Net::NetClientScriptEngine::onSymbolInstanceAssigned(unsigned sym, ZMemory::BigHandle serverHandle,
                                                          Daedalus::EInstanceClass instanceClass)
{
    using namespace Daedalus::GameState;

    Daedalus::DaedalusVM& vm = m_World.getScriptEngine().getVM();

    if(!getLocalHandle(instanceClass, serverHandle).isValid())
    {
        if(VERBOSE_LOGGING) LogWarn() << "Net: Server assigned symbol an instance unknown to the client! Serverhandle: " << serverHandle.index;
        return;
    }

    vm.getDATFile().getSymbolByIndex(sym).instanceDataHandle = getLocalHandle(instanceClass, serverHandle);
    vm.getDATFile().getSymbolByIndex(sym).instanceDataClass = instanceClass;
}

Handle::EntityHandle Net::NetClientScriptEngine::insertNPC(unsigned sym, const std::string& waypoint)
{
    Daedalus::DaedalusVM& vm = m_World.getScriptEngine().getVM();
    LogInfo() << "Net: Inserting NPC of instance " << vm.getDATFile().getSymbolByIndex(sym).name << " at " << waypoint;

    return VobTypes::Wld_InsertNpc(m_World, sym, waypoint);
}

void Net::NetClientScriptEngine::onNPCCreated(uint32_t sym, ZMemory::BigHandle serverhandle, std::string startpoint)
{
    using namespace Daedalus::GameState;

    Daedalus::DaedalusVM& vm = m_World.getScriptEngine().getVM();

    // Add the NPC to the current world
    Handle::EntityHandle e = VobTypes::Wld_InsertNpc(m_World, sym, startpoint);
    VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, e);
    NpcHandle localhandle = VobTypes::getScriptHandle(npc);

    // Map the new instance to the servers
    registerLocalHandle(Daedalus::IC_Npc, serverhandle, localhandle);
}

void Net::NetClientScriptEngine::readWorldSyncStream(bool wait)
{
    sfn::Message msg;

    if(wait)
    {
        LogInfo() << "Net: Waiting for world-sync";
        while (!getClientState().m_Link->Receive(WorldSyncStream, msg))
        {}; // TODO: Wait a few ms here
    }

    // Process futher messages
    do{

        while(msg.GetSize())
        {
            ScriptPacket p;
            msg >> p;

            switch(p)
            {
                case SP_NPC_Created:
                {
                    uint32_t sym;
                    ZMemory::BigHandle serverhandle;
                    std::string startpoint;

                    msg >> sym;
                    msg >> serverhandle;
                    msg >> startpoint;

                    onNPCCreated(sym, serverhandle, startpoint);
                }
                    break;

                case SP_NPC_Teleport:
                {
                    ZMemory::BigHandle serverhandle;
                    Math::float3 newPosition;
                    Math::float3 newDirection;

                    msg >> serverhandle;
                    msg >> newPosition;
                    msg >> newDirection;

                    onNPCTeleport(serverhandle, newPosition, newDirection);
                }
                    break;

                case SP_NPC_Killed:
                {
                    ZMemory::BigHandle killed;
                    ZMemory::BigHandle killer;

                    msg >> killed;
                    msg >> killer;

                    onNPCKilled(killed, killer);
                }
                    break;

                case SP_Item_Created:
                {
                    Handle::EntityHandle serverhandle;
                    uint32_t sym;
                    Math::Matrix transform;

                    msg >> serverhandle;
                    msg >> sym;
                    msg >> transform;

                    onItemInserted(serverhandle, sym, transform);
                }
                    break;
                default:
                    ; // Not interesting
            }
        }
    }while(getClientState().m_Link->Receive(WorldSyncStream, msg));

}

void Net::NetClientScriptEngine::onNPCTeleport(ZMemory::BigHandle serverhandle, const Math::float3& newPosition,
                                               const Math::float3& newDirection)
{
    ZMemory::BigHandle h = getLocalHandle(Daedalus::IC_Npc, serverhandle);

    if(!h.isValid())
        return;

    VobTypes::NpcVobInformation npc = VobTypes::getVobFromScriptHandle(m_World, ZMemory::handleCast<Daedalus::GameState::NpcHandle>(h));

    if(!npc.isValid())
        return;

    npc.playerController->setPositionAndDirection(newPosition, newDirection);
}

void Net::NetClientScriptEngine::onNPCKilled(ZMemory::BigHandle killed, ZMemory::BigHandle killer)
{
    using namespace Daedalus::GameState;

    ZMemory::BigHandle localKilled = getLocalHandle(Daedalus::IC_Npc, killed);
    ZMemory::BigHandle localKiller = getLocalHandle(Daedalus::IC_Npc, killer);

    if(!localKilled.isValid())
        return;

    VobTypes::NpcVobInformation killedNPC = VobTypes::getVobFromScriptHandle(m_World,ZMemory::handleCast<NpcHandle>(localKilled));

    if(!killedNPC.isValid())
        return;

    if(localKiller.isValid())
    {
        // Killer is known
        VobTypes::NpcVobInformation killerNPC = VobTypes::getVobFromScriptHandle(m_World,
                                                                                 ZMemory::handleCast<NpcHandle>(
                                                                                         localKiller));

        LogInfo() << "Net: Killed by " << VobTypes::getScriptObject(killerNPC).name[0] << ": " << VobTypes::getScriptObject(killedNPC).name[0];
        killedNPC.playerController->die(killerNPC.entity);
    } else
    {
        // Killer is unknown

        LogInfo() << "Net: Killed: " << VobTypes::getScriptObject(killedNPC).name[0];
        killedNPC.playerController->die(Handle::EntityHandle::makeInvalidHandle());
    }


}

void Net::NetClientScriptEngine::onItemInserted(Handle::EntityHandle serverhandle, unsigned sym, const Math::Matrix& transform)
{
    LogInfo() << "Net: Server inserted item of instance " << sym << " at " << transform.Translation().toString();

    Handle::EntityHandle e = VobTypes::createItem(m_World, sym);
    VobTypes::ItemVobInformation item = VobTypes::asItemVob(m_World, e);

    Vob::setTransform(item, transform);

    // Link to servers entity
    registerLocalHandle(EntityType::ET_Item, serverhandle, e);
}

void Net::NetClientScriptEngine::removeLocalEntity(Net::EntityType type, Handle::EntityHandle localHandle)
{
    auto& instMap = m_EntityMap[type];
    auto& instMapLocal = m_EntityMapLocal[type];

    Handle::EntityHandle serverHandle = getServerEntity(type, localHandle);

    instMap.erase(serverHandle);
    instMapLocal.erase(localHandle);
}

void Net::NetClientScriptEngine::onItemTaken(Handle::EntityHandle serverhandle)
{
    Handle::EntityHandle local = getLocalEntity(EntityType::ET_Item, serverhandle);


    if(local.isValid())
    {
        VobTypes::ItemVobInformation item = VobTypes::asItemVob(m_World, local);

        if(item.isValid())
        {
            item.itemController->remove();

            LogInfo() << "Net: Server removed item (Entity: " << serverhandle.index << ")";
        }
    }
}

void Net::NetClientScriptEngine::onNPCAddInventory(ZMemory::BigHandle serverhandle, unsigned sym, int count)
{
    using namespace Daedalus::GameState;

    ZMemory::BigHandle hlocalNPC = getLocalHandle(Daedalus::IC_Npc, serverhandle);

    if(!hlocalNPC.isValid())
        return;

    VobTypes::NpcVobInformation localNPC = VobTypes::getVobFromScriptHandle(m_World,ZMemory::handleCast<NpcHandle>(hlocalNPC));

    if(!localNPC.isValid())
        return;

    if(count > 0)
        localNPC.playerController->getInventory().addItem(sym, (unsigned)count);
    else if(count < 0)
        localNPC.playerController->getInventory().removeItem(sym, (unsigned)(-count));
}





