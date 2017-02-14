#pragma once 
#include <deque>
#include <logic/SyncedObjects.h>
#include <SFNUL.hpp>
#include "NetClientScriptEngine.h"

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
    class ClientState
    {
        friend class NetClientScriptEngine;
    public:
        ClientState(Engine::NetEngine& engine, World::WorldInstance& world);
        ~ClientState();

        /**
         * Connects to a server at the given ip/port
         * @param ip IP of the server
         * @param port Port of the server
         * @return success
         */
        bool connect(const std::string& ip, uint16_t port);

        /**
         * Called when the world was loaded for this client
         */
        void onWorldLoaded();

        /**
         * Called every frame
         * @param deltaTime Time since last frame
         */
        void update(float deltaTime);

        /**
         * Sends a message to the servers chatlog
         * @param message Message to send
         */
        void sendMessage(const std::string& message);

        /**
         * @return True if the client is currently connected and logged in
         */
        bool isConnected();

        /**
         * Sends message for when an NPC was killed by this client
         * @param killed The killed NPC
         * @param attackingNPC the killing NPC
         */
        void onNPCKilled(ZMemory::BigHandle killed);

        /**
         * Sends a message for when the given Item is to be taken.
         * @param item Item that is to be taken
         */
        void onItemTaken(Handle::EntityHandle item);
    protected:

        /**
         * Spawns an NPC to control and links it to the servers instance
         */
        void setupPlayer();

        /**
         * Gets updates from the server
         */
        void readStreams();

        /**
         * Registers all multiplayer related console commands
         */
        void registerConsoleCommands();

        /**
         * Displays the latest chat-messages using debug-drawing
         */
        void debugDisplayChat();

        /**
         * World/Engine this is updating
         */
        World::WorldInstance& m_World;
        Engine::NetEngine& m_Engine;

        /**
         * This clients link to the server
         */
        typedef std::shared_ptr<sfn::Link<sfn::TcpSocket>> TcpLink; 
        TcpLink m_Link;

        /**
         * Clients synchronizer
         */
        sfn::SynchronizerClient m_Synchronizer;

        /**
         * State of all registered players
         */
        std::deque<Net::PlayerState> m_Players; 

        /**
         * Pointer to the player on this machine
         */
        Net::PlayerState* m_ThisPlayer;

        /**
         * This clients ID
         */
        uint16_t m_PlayerID;

        /**
         * NPC-Handle on the server this client should control
         */
        ZMemory::BigHandle m_AssignedNPC;

        /**
         * Name selected by the player
         */
        std::string m_Name;

        /**
         * Chatlog
         */
        Chatlog m_ChatLog;

        /**
         * Whether we are currently connected
         */
        bool m_IsConnected;

        /**
         * Script engine sync
         */
        NetClientScriptEngine m_ScriptEngine;
    };
}
