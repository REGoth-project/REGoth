#include "ClientState.h"
#include <utils/logger.h>
#include <components/VobClasses.h>
#include <logic/ScriptEngine.h>
#include <logic/PlayerController.h>
#include <components/VobClasses.h>
#include <utils/cli.h>
#include <engine/BaseEngine.h>
#include <ui/Hud.h>
#include <ui/Console.h>
#include <engine/NetEngine.h>

namespace Flags
{
    Cli::Flag username("name", "", 1, "Username, when in client-mode", {Utils::getUserName()}, "Multiplayer");
}

Net::ClientState::ClientState(Engine::NetEngine& engine, World::WorldInstance& world) :
        m_World(world),
        m_Engine(engine),
        m_ScriptEngine(engine, world),
        m_PlayerID(0)
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

void Net::ClientState::registerConsoleCommands()
{
    auto& console = m_Engine.getHud().getConsole();
    console.registerCommand("say", [&](const std::vector<std::string>& args) -> std::string {

        if(args.size() < 2)
            return "Missing argument. Usage: say <msg>";

        std::string s;

        // un-split
        for(size_t i=1;i<args.size();i++)
            s += args[i];

        sendMessage(s);

        return "";
    });

    console.registerCommand("ckill", [&](const std::vector<std::string>& args) -> std::string {

        if(args.size() < 2)
            return "Missing argument. Usage: ckill <npc-name>";

        std::string s;

        // un-split
        for(size_t i=1;i<args.size();i++)
            s += args[i];

        sendMessage(s);

        return "";
    });
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

    m_Synchronizer.SetLifetimeManagers(

            // For the Chatlog
            Net::Chatlog::type_id,

            // Factory
            [&](){
                LogInfo() << "Net: Creating chatlog";

                m_ChatLog = std::move( Chatlog() );

                return &m_ChatLog;
            },

            // Destructor
            [&](sfn::SyncedObject* player)
            {
                // Nothing to do here
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
            msg << Flags::username.getParam(0);

            if(m_Link->Send(StreamID::LoginStream, msg))
            {
                sfn::Message reply;

                LogInfo() << "Net: Waiting for player-ID...";
                while(!m_Link->Receive(StreamID::LoginStream, reply))
                {
                    // Server maybe already sending other messages
                    readStreams();
                } // TODO: Only wait for a little




                LoginPacket type;
                reply >> type;

                if(type != LoginPacket::LP_AskPlayerID)
                {
                    LogError() << "Net: LP_AskPlayerID failed from server!";
                    exit(0);
                }

                uint16_t playerID;
                reply >> playerID;
                reply >> m_AssignedNPC;

                // Wait for this clients player-object to be created
                LogInfo() << "Net: Waiting for player-object...";
                while(m_PlayerID == 0)
                {
                    readStreams();
                    for (Net::PlayerState& p : m_Players)
                    {
                        if (p.getPlayerID() == playerID)
                        {
                            m_PlayerID = playerID;
                            break;
                        }
                    }
                }




                LogInfo() << "Net: Got player-ID " << m_PlayerID << " and NPC " << m_AssignedNPC.index;


            } else
            {
                LogError() << "Net: Failed to send login-message!";
                exit(0);
            }
        }
    }

    registerConsoleCommands();

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

    readStreams();

    bgfx::dbgTextPrintf(0, 5, 0x4f, "Online:");

    // Make sure the client has an NPC to move
    if(!m_ThisPlayer)
    {
        setupPlayer();
    }

    int i=1;
    for(Net::PlayerState& p : m_Players)
    {
        // Need to create a new player-entity for newly joined players
        if(!p.getPlayerEntity().isValid() && &p != m_ThisPlayer)
        {
            // Create new hero-instance
            Handle::EntityHandle e = VobTypes::Wld_InsertNpc(m_World, "PC_HERO", "WP_INTRO_FALL3");
            VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, e);

            // Set name
            VobTypes::getScriptObject(npc).name[0] = p.getName();

            p.setPlayerEntity(e);

            LogInfo() << "Net: Created hero instance " << e.index << " for player '" << p.getName() << "'";
        }


        bgfx::dbgTextPrintf(0, 5+i, 0x0f, " %s", p.getName().c_str());
        p.onFrameUpdate(deltatime, m_World, &p == m_ThisPlayer);

        i++;
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

    debugDisplayChat();
}

void Net::ClientState::sendMessage(const std::string& message)
{
    sfn::Message msg;
    msg << ChatPacket::CP_Message;
    msg << message;

    m_Link->Send(StreamID::ChatStream, msg);
}

void Net::ClientState::debugDisplayChat()
{
    const int MAX_NUM_MESSAGES = 10;

    int i=0;
    for(const std::string& m : m_ChatLog.getMessages())
    {
        bgfx::dbgTextPrintf(0, 25+i, 0x0f, " %s", m.c_str());
        i++;

        if(i > MAX_NUM_MESSAGES)
            break;
    }
}

void Net::ClientState::readStreams()
{
    // Update script engine
    m_ScriptEngine.readStreams();

    m_Synchronizer.Update();
}




bool Net::ClientState::isConnected()
{
    return m_PlayerID != 0;
}

void Net::ClientState::setupPlayer()
{
    // Create player-instance on client
    // Note: Players NPC has to be the first one in the world so it gets set as player-entity!
    Handle::EntityHandle e = VobTypes::Wld_InsertNpc(m_World, "PC_HERO", "WP_INTRO_FALL3");
    LogInfo() << "Net: Inserted Player Character (Entity " << e.index << ")";

    // Get the script-handle of the created NPC
    VobTypes::NpcVobInformation clientNPC = VobTypes::asNpcVob(m_World, e);
    Daedalus::GameState::NpcHandle hclientNPC = VobTypes::getScriptHandle(clientNPC);

    // Set name
    VobTypes::getScriptObject(clientNPC).name[0] = m_Name;

    // And link it to the servers instance
    m_ScriptEngine.registerLocalHandle(Daedalus::IC_Npc, m_AssignedNPC, hclientNPC);

    // Find "this-player" and assign NPC-instance to it
    for (Net::PlayerState& p : m_Players)
    {
        if (p.getPlayerID() == m_PlayerID)
        {
            m_ThisPlayer = &p;
            m_ThisPlayer->setPlayerEntity(e);
            break;
        }
    }

    if (!m_ThisPlayer)
    {
        LogError() << "Net: Couldn't find player with this clients player-id";
        exit(0);
    }

    // Wait for world-data to come in
    m_ScriptEngine.readWorldSyncStream(true);
}

void Net::ClientState::onNPCKilled(ZMemory::BigHandle killed)
{
    sfn::Message msg;

    ZMemory::BigHandle serverHandle = m_ScriptEngine.getServerHandle(Daedalus::IC_Npc, killed);

    if(!serverHandle.isValid())
    {
        LogWarn() << "Net: Killed NPC is unknown by server / client not in sync!";
        return;
    }

    msg << PlayerActionPacket::PA_NPC_Killed;
    msg << serverHandle;

    m_Link->Send(PlayerActionsStream, msg);
}