#pragma once
#include <SFNUL.hpp>
#include <deque>
#include <logic/SyncedObjects.h>

namespace World
{
    class WorldInstance;
}

namespace Net
{
    class ServerState
    {
    public:
        ServerState(World::WorldInstance& world);
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

    protected:

        /**
         * World this is the state of
         */
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
         * Server-synchronizer
         */
        sfn::SynchronizerServer m_Synchronizer;

        /**
         * Connected clients
         */
        typedef std::shared_ptr<sfn::Link<sfn::TcpSocket>> TcpLink;

        struct Client
        {
            Client(TcpLink l) : link(l) {}

            TcpLink link;
            Net::PlayerState* player;
        };

        std::deque<Client> m_Links;

        /**
         * Temporary link to accept pending connections
         */
        TcpLink m_PendingConnectionLink;
    };
}
