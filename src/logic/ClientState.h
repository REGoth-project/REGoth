#pragma once 
#include <deque>
#include <logic/SyncedObjects.h>
#include <SFNUL.hpp>

namespace World
{
    class WorldInstance;
}

namespace Net
{
    class ClientState
    {
    public:
        ClientState(World::WorldInstance& world);
        ~ClientState();

        /**
         * Connects to a server at the given ip/port
         * @param ip IP of the server
         * @param port Port of the server
         * @return success
         */
        bool connect(const std::string& ip, uint16_t port);

        /**
         * Called every frame
         * @param deltaTime Time since last frame
         */
        void update(float deltaTime);

    protected:

        /**
         * World this is updating
         */
        World::WorldInstance& m_World;

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
         * Whether we are currently connected
         */
        bool m_IsConnected;
    };
}
