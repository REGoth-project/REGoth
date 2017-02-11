#include "ServerState.h"
#include <utils/logger.h>
#include <components/VobClasses.h>
#include <logic/PlayerController.h>

Net::ServerState::ServerState(World::WorldInstance& world) : 
    m_World(world)
{
    
}

Net::ServerState::~ServerState()
{
    // Gracefully close all connections.
    for( auto& l : m_Links) 
    {
        if( l.link && ( l.link->GetTransport() && l.link->GetTransport()->IsConnected() ) )
        {
            l.link->Shutdown();
        }
    }

    // Stop all networking threads
    sfn::Stop();
}

bool Net::ServerState::startListening(uint16_t port)
{
    LogInfo() << "Net: Listening on port: " << port;

    m_Listener = sfn::TcpListener::Create();

    m_Listener->Listen(sfn::Endpoint{sfn::IpAddress{"0.0.0.0"}, port});

    // Start networking thread
    LogInfo() << "Net: Starting networking thread";
    sfn::Start();

    m_PendingConnectionLink = std::make_shared<sfn::Link<sfn::TcpSocket>>();

    sfn::SetStreamSynchronizationPeriod(std::chrono::milliseconds(100));

    return true;
}

void Net::ServerState::update(float deltatime)
{
    // Check for pending connections
    do {
        // Accept all pending connections and bind them to the temporary Link.
        m_PendingConnectionLink->SetTransport( m_Listener->GetPendingConnection() );

        // If the Link is connected...
        if( m_PendingConnectionLink->GetTransport() && m_PendingConnectionLink->GetTransport()->IsConnected() ) 
        {
            LogInfo() << "Net: New client connected!";

            // Add it to the synchronizer as a new client.
            m_Synchronizer.AddClient( m_PendingConnectionLink );

            // Move it into our housekeeping container and create a new temporary.
            m_Links.emplace_back( std::move( m_PendingConnectionLink ) );

            m_PendingConnectionLink = std::make_shared<sfn::Link<sfn::TcpSocket>>();

            // Figure out player-id of the new player
            uint16_t lastPlayerID = m_Players.empty() ? (uint16_t)0 : m_Players.back().getPlayerID();

            // Create a player for that client
            m_Players.emplace_back(m_Synchronizer.CreateObject<Net::PlayerState>());
            m_Links.back().player = &m_Players.back();

            // Create new hero-instance

            Handle::EntityHandle e = VobTypes::Wld_InsertNpc(m_World, "PC_HERO", "WP_INTRO_FALL3");
            LogInfo() << "Net: Inserted Character (Entity " << e.index << ")";

            m_Players.back().setPlayerID((uint16_t)(lastPlayerID + 1));
            m_Players.back().setPlayerEntity(e);

            LogInfo() << "Net: New client got player-id " << m_Players.back().getPlayerID();

            // Client should be asking for his player-ID now
            sfn::Message askPlayerID;
            while(!(m_Links.back().link->Receive(StreamID::LoginStream, askPlayerID))); // TODO: Only wait for a little

            {
                LoginPacket p;
                askPlayerID >> p;

                if(p != LoginPacket::LP_AskPlayerID)
                {
                    // Respond with error
                    LogWarn() << "Net: Player did not send Player-ID request but rather packet-id " << p << "!";
                    sfn::Message err;
                    err << LoginPacket::LP_Error;

                    m_Links.back().link->Send(StreamID::LoginStream, err);
                }else
                {
                    sfn::Message reply;
                    reply << LoginPacket::LP_AskPlayerID;
                    reply << m_Players.back().getPlayerID();

                    if(!m_Links.back().link->Send(StreamID::LoginStream, reply))
                        LogWarn() << "Net: Failed to reply with Player-ID!";
                    else
                        LogInfo() << "Net: Replied with player-ID " << m_Players.back().getPlayerID();
                }
            }
        }

    } while( m_PendingConnectionLink->GetTransport() && m_PendingConnectionLink->GetTransport()->IsConnected() );

    // Get rid of disconnected Links. The synchronizer will get rid
    // of dead Links automatically. 
    for( auto iter = std::begin( m_Links ); iter != std::end( m_Links ); ) {
        auto transport = ( *iter ).link->GetTransport();
        if( !transport || !transport->IsConnected() || transport->RemoteHasShutdown() ) {
            transport->Shutdown();
            iter = m_Links.erase( iter );
            continue;
        }
        ++iter;
    }

    for(auto& l : m_Links)
    {
        sfn::Message message;
        while(l.link->Receive(Net::StreamID::PlayerInputStream, message))
        {
            Logic::PlayerInput input;
            message >> input;

            if(l.player)
                l.player->setPlayerInput(input);
        }
    }

    for(Net::PlayerState& p : m_Players)
    {
        p.onFrameUpdate(deltatime, m_World);

        // This needs to be sourced out to the specific controllers
        VobTypes::NpcVobInformation pvob = VobTypes::asNpcVob(m_World, p.getPlayerEntity());

        if(pvob.isValid())
        {
            // Get new position/direction
            p.getPosition() = pvob.playerController->getEntityTransform().Translation();
            p.getDirection() = pvob.playerController->getDirection();
        }

    }

    // Broadcast the current state
    m_Synchronizer.Update();
}
