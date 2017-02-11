#include "ClientState.h"
#include <utils/logger.h>
#include <components/VobClasses.h>
#include <logic/ScriptEngine.h>
#include <logic/PlayerController.h>
#include <components/VobClasses.h>

Net::ClientState::ClientState(World::WorldInstance& world) : m_World(world)
{
    m_IsConnected = false;
    m_ThisPlayer = nullptr;
}

Net::ClientState::~ClientState()
{
    // Gracefully close all connections.
    if( m_Link && ( m_Link->GetTransport() && m_Link->GetTransport()->IsConnected() ) ) {
        m_Link->Shutdown();

        while( !m_Link->RemoteHasShutdown() ) {
            // TODO: Wait a few ms here
        }
    }

    // Stop all network processing threads.
    sfn::Stop();
}


bool Net::ClientState::connect(const std::string& ip, uint16_t port)
{
    m_Link = std::make_shared<sfn::Link<sfn::TcpSocket>>( sfn::TcpSocket::Create() );

    LogInfo() << "Net: Trying to connect to " << ip << ":" << port;

    m_Synchronizer.SetLifetimeManagers(

            // For the PlayerState
            Net::PlayerState::type_id,

            // Factory
            [&](){

                LogInfo() << "Net: New player joined";
                m_Players.emplace_back();


                return &m_Players.back();
            },

            // Destructor
            [&](sfn::SyncedObject* player)
            {
                if(player == m_ThisPlayer)
                    m_ThisPlayer = nullptr;

                auto iter = std::find_if( std::begin( m_Players ), std::end( m_Players ),
					[player]( const Net::PlayerState& element ) {
						return player == &element;
					}
				);

				if( iter != std::end( m_Players ) ) {
					m_Players.erase( iter );
				}

                // TODO: Remove player entity

            }
        );


    m_Link->Connect(sfn::Endpoint{ip, port});
    
    // Start networking thread
    sfn::Start();

    // Try to connect to the server
    while(!m_IsConnected)
    {
        // If we aren't already connected and the Link just came alive,
        // add it to the synchronizer as a server and set connected to true.
        if( !m_IsConnected && m_Link->GetTransport() && m_Link->GetTransport()->IsConnected() ) {
            m_Synchronizer.AddServer( m_Link );
            m_IsConnected = true;

            // Ask for player ID
            sfn::Message msg;
            msg << LoginPacket::LP_AskPlayerID;

            if(m_Link->Send(StreamID::LoginStream, msg))
            {
                sfn::Message reply;

                LogInfo() << "Net: Waiting for player-ID...";
                while(!m_Link->Receive(StreamID::LoginStream, reply))
                {
                    // Server maybe already sending other messages
                    m_Synchronizer.Update();
                } // TODO: Only wait for a little

                LoginPacket type;
                reply >> type;

                if(type != LoginPacket::LP_AskPlayerID)
                {
                    LogError() << "Net: LP_AskPlayerID failed from server!";
                    exit(0);
                }

                reply >> m_PlayerID;
                LogInfo() << "Net: Got player-ID " << m_PlayerID;
            } else
            {
                LogError() << "Net: Failed to send login-message!";
                exit(0);
            }
        }
    } 

    return true;
}

void Net::ClientState::update(float deltatime)
{
    if(m_IsConnected)
    {
        // If we are already connected and the Link dies, gracefully
        // shut down and exit the game loop.
        auto transport = m_Link->GetTransport();

        if( ( !transport || !transport->IsConnected() || transport->RemoteHasShutdown() ) ) {
            m_Link->Shutdown();
            m_IsConnected = false;

            return;
        }
    }



    m_Synchronizer.Update();

    int i=0;
    for(Net::PlayerState& p : m_Players)
    {
        if(!m_ThisPlayer && p.getPlayerID() == m_PlayerID)
            m_ThisPlayer = &p;

        // Need to create a new player-entity for newly joined players
        if(!p.getPlayerEntity().isValid() && &p != m_ThisPlayer)
        {
            // Create new hero-instance
            Handle::EntityHandle e = VobTypes::Wld_InsertNpc(m_World, "PC_HERO", "WP_INTRO_FALL3");
            p.setPlayerEntity(e);

            LogInfo() << "Net: Created hero instance " << e.index;
        }


        bgfx::dbgTextPrintf(0, 5+i, 0x4f, "Updating instance %d (Forward: %d)", p.getPlayerEntity().index, p.getPlayerInput().isForward ? 1 : 0);
        p.onFrameUpdate(deltatime, m_World, &p == m_ThisPlayer);
    }

    // Send new input
    if(m_ThisPlayer)
    {
        if(!m_ThisPlayer->getPlayerEntity().isValid())
            m_ThisPlayer->setPlayerEntity(m_World.getScriptEngine().getPlayerEntity());

        if(m_ThisPlayer->getPlayerEntity().isValid())
        {
            VobTypes::NpcVobInformation p = VobTypes::asNpcVob(m_World, m_ThisPlayer->getPlayerEntity());

            if(p.isValid())
            {
                Logic::PlayerInput input = p.playerController->temp_GetInputInfo();

                sfn::Message msg;
                msg << input;

                if( msg.GetSize() ) {
                    m_Link->Send( StreamID::PlayerInputStream, msg );
                }
            }
        }
    }
}
