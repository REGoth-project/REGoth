//
// Created by desktop on 12.02.17.
//

#pragma once

#include <ZenLib/daedalus/DATFile.h>
#include <SFNUL/include/SFNUL/Message.hpp>
#include <handle/HandleDef.h>
#include <math/mathlib.h>
#include <logic/SyncedObjects.h>

namespace World
{
    class WorldInstance;
}

namespace Engine
{
    class NetEngine;
}

namespace Net
{
    class ClientState;

    /**
     * Keeps the local instances up-to date
     */
    class NetClientScriptEngine
    {
    public:
        NetClientScriptEngine(Engine::NetEngine& engine, World::WorldInstance& world);
        ~NetClientScriptEngine();

        /**
         * Reads the link of the local player for any script related data
         * Called per frame.
         */
        void readStreams();

        /**
         * Checks if any world-syncs came from the server
         * @param wait Whether the function should wait until actual data comes in
         */
        void readWorldSyncStream(bool wait = true);

        /**
         * Looks up the local version of the given script handle
         * @param instanceClass Class to look up
         * @param serverHandle Handle on the server we want to know the local version of
         * @return Local handle. Invalid if couldn't be found.
         */
        ZMemory::BigHandle getLocalHandle(Daedalus::EInstanceClass instanceClass, ZMemory::BigHandle serverHandle);
        ZMemory::BigHandle getServerHandle(Daedalus::EInstanceClass instanceClass, ZMemory::BigHandle localHandle);

        /**
         * Registers the local version of a server-object here
         * @param instanceClass Class of the object
         * @param serverHandle Handle on the server
         * @param localHandle Local handle of this client
         */
        void registerLocalHandle(Daedalus::EInstanceClass instanceClass,
                                 ZMemory::BigHandle serverHandle,
                                 ZMemory::BigHandle localHandle);

        template<typename T>
        void registerLocalHandle(Daedalus::EInstanceClass instanceClass,
                                 ZMemory::BigHandle serverHandle,
                                 T localHandle)
        {
            registerLocalHandle(instanceClass, serverHandle, ZMemory::toBigHandle(localHandle));
        }

        /**
         * Looks up the local version of the given script handle
         * @param instanceClass Class to look up
         * @param serverHandle Handle on the server we want to know the local version of
         * @return Local handle. Invalid if couldn't be found.
         */
        Handle::EntityHandle getLocalEntity(EntityType type, Handle::EntityHandle serverHandle);
        Handle::EntityHandle getServerEntity(EntityType type, Handle::EntityHandle localHandle);

        /**
         * Registers the local version of a server-object here
         * @param instanceClass Class of the object
         * @param serverHandle Handle on the server
         * @param localHandle Local handle of this client
         */
        void registerLocalHandle(EntityType type,
                                 Handle::EntityHandle serverHandle,
                                 Handle::EntityHandle localHandle);

        /**
         * Removes the tracking of a local handle to it's server counterpart
         * @param type Type of the handle
         * @param localHandle local handle to remove tracking of
         */
        void removeLocalEntity(EntityType type, Handle::EntityHandle localHandle);

    protected:

        /**
         * Called from the server to insert an NPC at the given location
         * @param sym Symbol of the NPC to be inserted
         * @param waypoint Waypoint to put at
         * @return Handle to the created entity
         */
        Handle::EntityHandle insertNPC(unsigned sym, const std::string& waypoint);

        /**
         * @return Client-state object
         */
        ClientState& getClientState();

        /**
         * Callback for when a script-symbol got changed. (Plain data)
         * @param sym Changed Symbol-index
         * @param op Op that changed the symbol so we know the datatype
         * @param newSym Server-version of that symbol
         */
        void onSymbolChanged(unsigned sym, Daedalus::EParOp op, const Daedalus::PARSymbol& newSym);

        /**
         * Callback for when a data-field of an instance got changed
         * @param serverHandle Handle of the instance on the server
         * @param instanceClass Class of the instance
         * @param data Rest of the data-packet. (Actual instance-data)
         */
        void onInstanceDataChanged(ZMemory::BigHandle serverHandle, Daedalus::EInstanceClass instanceClass, sfn::Message& data);

        /**
         * Callback for when the instance held by a symbol changed on the server
         * @param sym Symbol that got changed
         * @param serverHandle New handle on the server
         * @param instanceClass Class of the new handle
         */
        void onSymbolInstanceAssigned(unsigned sym, ZMemory::BigHandle serverHandle, Daedalus::EInstanceClass instanceClass);



        /**
         * Callback for when an actual instance was created/removed
         * @param handle Handle to the created instance
         * @param instanceClass Class of the created instance
         */
        void onInstanceCreated(ZMemory::BigHandle serverHandle, Daedalus::EInstanceClass instanceClass);
        void onInstanceRemoved(ZMemory::BigHandle serverHandle, Daedalus::EInstanceClass instanceClass);

        /**
         * Callback for when an NPC got created on the server
         * @param sym Instance of the npc
         * @param serverhandle Handle on the server
         * @param startpoint Waypoint to insert it onto
         */
        void onNPCCreated(uint32_t sym, ZMemory::BigHandle serverhandle, std::string startpoint);

        /**
         * Callback for when an NPC got killed on the server
         * @param killed (Serverhandle) killed npc
         * @param killer (Serverhandle) killer
         */
        void onNPCKilled(ZMemory::BigHandle killed, ZMemory::BigHandle killer);

        /**
         * Callback for when an NPC should be teleported by the server
         * @param serverhandle Servers handle of the NPC
         * @param newPosition New postion of the NPC
         * @param newDirection New direction of the NPC
         */
        void onNPCTeleport(ZMemory::BigHandle serverhandle, const Math::float3& newPosition, const Math::float3& newDirection);

        /**
         * Callback for when an Item was inserted into the world
         * @param sym Symbol-index of the inserted item
         * @param transform Transform of the item
         */
        void onItemInserted(Handle::EntityHandle serverhandle, unsigned sym, const Math::Matrix& transform);

        /**
         * Callback for when an Item was taken by some player (removes it from the world)
         * @param serverhandle Serverhandle of the taken item
         */
        void onItemTaken(Handle::EntityHandle serverhandle);

        /**
         * Callback for when the server wants to add something to the inventory of a player
         * Note: Count can be negative to remove items
         * @param serverhandle NPC to give/take the items
         * @param sym Instance of the item to give/take
         * @param count Number of items to give/take (can be negative)
         */
        void onNPCAddInventory(ZMemory::BigHandle serverhandle, unsigned sym, int count);

        /**
         * Mappings for the handles on the server
         * Class -> (Server handle -> Local handle)
         */
        std::map<Daedalus::EInstanceClass, std::map<ZMemory::BigHandle, ZMemory::BigHandle>> m_InstanceMap;

        /**
         * Mappings for the handles on the server
         * Class -> (Local handle -> Server handle)
         */
        std::map<Daedalus::EInstanceClass, std::map<ZMemory::BigHandle, ZMemory::BigHandle>> m_InstanceMapLocal;

        /**
         * Mappings for the handles on the server
         * Class -> (Server handle -> Local handle)
         */
        std::map<EntityType, std::map<Handle::EntityHandle, Handle::EntityHandle>> m_EntityMap;

        /**
         * Mappings for the handles on the server
         * Class -> (Local handle -> Server handle)
         */
        std::map<EntityType, std::map<Handle::EntityHandle, Handle::EntityHandle>> m_EntityMapLocal;

        /**
         * Engine and world this is running on
         */
        Engine::NetEngine& m_Engine;
        World::WorldInstance& m_World;
    };
}



