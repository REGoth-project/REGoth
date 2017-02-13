#include <engine/NetEngine.h>
#include <utils/cli.h>
#include <utils/logger.h>
#include <logic/ServerState.h>
#include <logic/ClientState.h>

using namespace Engine;

namespace Net
{
    // Used as extern, so other code can check this
    bool isServer = false;
    bool isClient = false;
    bool isNet = false;
}

namespace Flags
{
    Cli::Flag mpmode("mp", "multiplayer", 1, "[server|client], if passed, this will function as either a server or a client. Note that for a client, you must also supply the -ip and -port flags and for the server the -port flag.");

    Cli::Flag ip("ip", "", 1, "IP to connect to in client-mode");
    Cli::Flag port("port", "", 1, "Port to listen at/the server listens at"); 
}

NetEngine::NetEngine()
{

}

NetEngine::~NetEngine()
{

}

void NetEngine::initEngine(int argc, char** argv)
{
    GameEngine::initEngine(argc, argv);

    if(Flags::mpmode.isSet())
    {
        if(Flags::mpmode.getParam(0) == "server")
        {
            if(!Flags::port.isSet())
            {
                LogError() << "Server needs to have the -port flag set!";
                exit(0);
            }
        }else if(Flags::mpmode.getParam(0) == "client")
        {
            if(!Flags::port.isSet() || !Flags::ip.isSet())
            {
                LogError() << "Client needs to have the -ip and -port flags set!";
                exit(0);
            }
        }else 
        {
            LogError() << "Invalid parameter for -mp flag! Valid are: server, client";
            exit(0);
        }
    }else
    {
        LogError() << "NetEngine needs to have -mp flag set!";
        exit(0);
    }
}

void NetEngine::onWorldCreated(Handle::WorldHandle world)
{
    GameEngine::onWorldCreated(world);
    setMainWorld(world);
    
    if(m_Server || m_Client)
    {
        LogError() << "Net: World switch not supported!";
        exit(0);
    }

    Net::isNet = true;

    if(Flags::mpmode.getParam(0) == "server")
    {
        Net::isServer = true;
        m_Server = new Net::ServerState(*this, world.get());
        m_Server->startListening(atoi(Flags::port.getParam(0).c_str()));

    }else 
    {
        Net::isClient = true;
        m_Client = new Net::ClientState(*this, world.get());
        m_Client->connect(Flags::ip.getParam(0), atoi(Flags::port.getParam(0).c_str()));
    }
}

void NetEngine::onWorldRemoved(Handle::WorldHandle world)
{
    GameEngine::onWorldRemoved(world);
    setMainWorld(Handle::WorldHandle::makeInvalidHandle());
}

void NetEngine::onFrameUpdate(double dt, uint16_t width, uint16_t height)
{
    if(m_Server)
        m_Server->update(dt);

    if(m_Client)
        m_Client->update(dt);

    GameEngine::onFrameUpdate(dt, width, height);
}

void NetEngine::loadArchives()
{
    GameEngine::loadArchives();
}

