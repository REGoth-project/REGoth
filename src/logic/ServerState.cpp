#include "ServerState.h"
#include <utils/logger.h>
#include <components/VobClasses.h>
#include <logic/PlayerController.h>
#include <engine/NetEngine.h>

const float SKYSTATE_SYNC_DELAY = 1.0f; // Seconds

Net::ServerState::ServerState(Engine::NetEngine& engine, World::WorldInstance& world) :
    m_World(world),
    m_Engine(engine),
    m_ScriptEngine(engine, world),
    m_SkyStateSync(SKYSTATE_SYNC_DELAY, [&](){ syncTime(); })
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

    m_ChatLog.reset( new Chatlog( std::move(m_Synchronizer.CreateObject<Chatlog>())));

    return true;
}

void Net::ServerState::update(float deltatime)
{
    static bool init = false;
    if(!init)
    {
        // Update scriptstate
        m_ScriptEngine.registerHooks();
        init = true;
    }

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

            auto& link = m_PendingConnectionLink;

            // Move it into our housekeeping container and create a new temporary.
            m_Links.emplace_back( std::move( m_PendingConnectionLink ) );

            m_PendingConnectionLink = std::make_shared<sfn::Link<sfn::TcpSocket>>();

            // Figure out player-id of the new player
            uint16_t lastPlayerID = m_Players.empty() ? (uint16_t)1 : m_Players.back().getPlayerID();

            // Create a player for that client
            m_Players.emplace_back(m_Synchronizer.CreateObject<Net::PlayerState>());
            m_Links.back().player = &m_Players.back();

            m_Players.back().setPlayerID((uint16_t) (lastPlayerID + 1));

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
                    std::string name;
                    askPlayerID >> name;

                    LogInfo() << "Net: Joined players name is '" << name << "'";
                    m_Players.back().getName() = name;

                    // Create new hero-instance
                    Handle::EntityHandle e = VobTypes::Wld_InsertNpc(m_World, "PC_HERO", "WP_INTRO_FALL3");
                    LogInfo() << "Net: Inserted Character (Entity " << e.index << ")";

                    m_Players.back().setPlayerEntity(e);

                    // Get the script-handle of the created NPC
                    VobTypes::NpcVobInformation clientNPC = VobTypes::asNpcVob(m_World, m_Players.back().getPlayerEntity());
                    Daedalus::GameState::NpcHandle hclientNPC = VobTypes::getScriptHandle(clientNPC);

                    sfn::Message reply;
                    reply << LoginPacket::LP_AskPlayerID;
                    reply << m_Players.back().getPlayerID();
                    reply << ZMemory::toBigHandle(hclientNPC);

                    if(!m_Links.back().link->Send(StreamID::LoginStream, reply))
                        LogWarn() << "Net: Failed to reply with Player-ID!";
                    else
                    {
                        // Make sure the players object arrives at him
                        m_Synchronizer.Update();

                        // Initial sync so that players world gets filled
                        syncWorldInitial(m_Links.back());

                        LogInfo() << "Net: Replied with player-ID " << m_Players.back().getPlayerID();
                        m_ChatLog->pushMessage(m_Players.back().getName() + " joined the game!");
                    }
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

        checkChatStream(l);
        checkPlayerActionStream(l);
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

    // Sync clients times and sky-states
    m_SkyStateSync.update((float)deltatime);

    // Broadcast the current state
    m_Synchronizer.Update();
}

void Net::ServerState::checkChatStream(Client& client)
{
    sfn::Message message;
    while(client.link->Receive(Net::StreamID::ChatStream, message))
    {
        ChatPacket p;
        message >> p;

        switch (p)
        {
            case ChatPacket::CP_Message:
            {
                std::string m;
                message >> m;

                if(m.size() < MAX_CHAT_MESSAGE_SIZE)
                {
                    LogInfo() << "Net: Got message: " << m;

                    // Add message to log
                    m_ChatLog->pushMessage(client.player->getName() + ": " + m);
                }
            }
                break;
            default:
                LogWarn() << "Net: ChatPacket invalid: " << p;
        }
    }
}

void Net::ServerState::checkPlayerActionStream(Net::ServerState::Client& client)
{
    sfn::Message message;
    while(client.link->Receive(Net::StreamID::PlayerActionsStream, message))
    {
        PlayerActionPacket p;
        message >> p;

        switch (p)
        {
            case PA_NPC_Killed:
            {
                // TODO: Should check if that player was really able to kill the NPC
                ZMemory::BigHandle killed;
                message >> killed;

                if(killed.isValid())
                {
                    VobTypes::NpcVobInformation npc = VobTypes::getVobFromScriptHandle(m_World,
                                                     ZMemory::handleCast<Daedalus::GameState::NpcHandle>(killed));

                    if(npc.isValid()
                       && npc.entity != m_World.getScriptEngine().getPlayerEntity()) // Don't let the main-entity be killed
                    {
                        LogInfo() << "Net: Player '" << client.player->getName() << "' killed " << VobTypes::getScriptObject(npc).name[0];

                        // Kill the NPC on server
                        VobTypes::NPC_Kill(m_World, npc, client.player->getPlayerEntity());
                    }
                }
            }
                break;
            default:
                LogWarn() << "Net: PlayerActionPacket invalid: " << p;
        }
    }
}

void Net::ServerState::broadcast(StreamID id, const sfn::Message& msg)
{
    for(auto& l : m_Links)
    {
        l.link->Send(id, msg);
    }
}

void Net::ServerState::syncWorldInitial(Client& client)
{
    // Go through every created NPC and send the regarding insert-commands
    for(Handle::EntityHandle e : m_World.getScriptEngine().getWorldNPCs())
    {
        VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, e);

        sfn::Message msg = onNPCInserted(ZMemory::toBigHandle(VobTypes::getScriptHandle(npc)),
                                         (unsigned)VobTypes::getScriptObject(npc).instanceSymbol,
                                         VobTypes::getScriptObject(npc).wp);

        client.link->Send(WorldSyncStream, msg);

        // Teleport to right position
        msg = onNPCTeleport(ZMemory::toBigHandle(VobTypes::getScriptHandle(npc)),
                            npc.playerController->getEntityTransform().Translation(),
                            npc.playerController->getDirection());

        client.link->Send(WorldSyncStream, msg);

        if(npc.playerController->getBodyState() == Logic::BS_DEAD)
        {
            msg = onNPCKilled(ZMemory::toBigHandle(VobTypes::getScriptHandle(npc)), ZMemory::BigHandle());

            client.link->Send(WorldSyncStream, msg);
        }
    }
}

sfn::Message Net::ServerState::onNPCInserted(ZMemory::BigHandle handle, unsigned sym, const std::string& wpname)
{
    sfn::Message msg;
    msg << Net::SP_NPC_Created;
    msg << (uint32_t)sym;
    msg << ZMemory::toBigHandle(handle);
    msg << wpname;

    return std::move( msg );
}

sfn::Message Net::ServerState::onNPCTeleport(ZMemory::BigHandle handle, const Math::float3& newPosition,
                                             const Math::float3& newDirection)
{
    sfn::Message msg;
    msg << Net::SP_NPC_Teleport;
    msg << handle;
    msg << newPosition;
    msg << newDirection;

    return std::move( msg );
}

sfn::Message Net::ServerState::onNPCKilled(ZMemory::BigHandle killed, ZMemory::BigHandle attackingNPC)
{
    sfn::Message msg;
    msg << Net::SP_NPC_Killed;
    msg << killed;
    msg << attackingNPC;

    return std::move( msg );
}

void Net::ServerState::syncTime()
{
    sfn::Message msg;
    msg << ScriptPacket::SP_WorldTimeSync;
    msg << m_World.getSky().getTimeOfDay();

    broadcast(StreamID::ScriptStream, msg);
}
