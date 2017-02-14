#pragma once
#include <SFNUL.hpp>
#include <deque>
#include <logic/SyncedObjects.h>
#include <logic/NetScriptEngine.h>
#include <utils/Utils.h>

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
    class ServerState
    {
        friend class NetScriptState;

    public:
        ServerState(Engine::NetEngine& engine, World::WorldInstance& world);
        ~ServerState();

        /**
         * Starts listening on the given port (server)
         * @param port to listen at
         * @return Success
         */
        bool startListening(uint16_t port);
        
        /**
         * One server tick
         * @param deltatime Time since last update
         */
        void update(float deltatime);

        /**
         * Sends the given message to all clients
         * @param msg Message to send
         */
        void broadcast(StreamID id, const sfn::Message& msg);

        /**
         * Creates a message which tells a client that the given NPC has just been spawned
         * @param handle Script handle of that NPC
         * @param sym Instance-symbol of the NPC
         * @param wpname Waypoint where it was inserted at
         */
        static sfn::Message onNPCInserted(ZMemory::BigHandle handle, unsigned sym, const std::string& wpname);

        /**
         * Creates a message for when NPC should be teleported by the server
         * @param serverhandle Servers handle of the NPC
         * @param newPosition New postion of the NPC
         * @param newDirection New direction of the NPC
         */
        static sfn::Message onNPCTeleport(ZMemory::BigHandle handle, const Math::float3& newPosition, const Math::float3& newDirection);

        /**
         * Creates a message for when an NPC was killed
         * @param killed The killed NPC
         * @param attackingNPC the killing NPC
         * @return message
         */
        static sfn::Message onNPCKilled(ZMemory::BigHandle killed, ZMemory::BigHandle attackingNPC);

        /**
         * Creates a message for when an Item was inserted
         * @param World entity of the item
         * @param sym Instance of the item
         * @return message
         */
        static sfn::Message onItemInserted(Handle::EntityHandle e, unsigned sym, const Math::Matrix& transform);

        /**
         * Creates a message for when an Item was removed from the world
         * @param World entity of the item
         * @return message
         */
        static sfn::Message onItemRemoved(Handle::EntityHandle e);

        /**
         * Callback for when the server wants to add something to the inventory of a player
         * Note: Count can be negative to remove items
         * @param serverhandle NPC to give/take the items
         * @param sym Instance of the item to give/take
         * @param count Number of items to give/take (can be negative)
         */
        static sfn::Message onNPCAddInventory(ZMemory::BigHandle serverhandle, unsigned sym, int count);
    protected:

        /**
         * Broadcasts the current time of day and weather to the clients
         */
        void syncTime();

        typedef std::shared_ptr<sfn::Link<sfn::TcpSocket>> TcpLink;
        struct Client
        {
            Client(TcpLink l) : link(l) {}

            TcpLink link;
            Net::PlayerState* player;
        };

        /**
         * Syncs the whole world with the given client.
         * Creates NPCs and Items
         */
        void syncWorldInitial(Client& client);

        /**
         * Checks for the various streams
         */
        void checkChatStream(Client& client);
        void checkPlayerActionStream(Client& client);

        /**
         * Engine and world this is running on
         */
        Engine::NetEngine& m_Engine;
        World::WorldInstance& m_World;

        /**
         * Servers listener socket
         */
        sfn::TcpListener::Ptr m_Listener;

        /**
         * State of all registered players
         */
        std::deque<Net::PlayerState> m_Players;

        /**
         * Current chatlog
         */
        std::unique_ptr<Net::Chatlog> m_ChatLog;

        /**
         * Server-synchronizer
         */
        sfn::SynchronizerServer m_Synchronizer;

        /**
         * Connected clients
         */
        std::deque<Client> m_Links;

        /**
         * Temporary link to accept pending connections
         */
        TcpLink m_PendingConnectionLink;

        /**
         * Script sync
         */
        NetScriptEngine m_ScriptEngine;

        /**
         * Skystate-sync
         */
        Utils::TimedAction m_SkyStateSync;
    };
}
