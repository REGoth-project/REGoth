//
// Created by desktop on 12.02.17.
//

#pragma once


#include <ZenLib/daedalus/DATFile.h>

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
    class ServerState;

    /**
     * Hooks into the regular script engine and keeps the clients scripts in sync
     */
    class NetScriptEngine
    {
        friend class ServerState;
    public:
        NetScriptEngine(Engine::NetEngine& engine, World::WorldInstance& world);
        ~NetScriptEngine();

        /**
         * To be called when the usual script-engine has been initialized
         */
        void registerHooks();

    protected:

        /**
         * @return Server-state object
         */
        ServerState& getServerState();

        /**
         * Callback for when a script-symbol got changed.
         * @param sym Changed Symbol-index
         * @param op Op that changed the symbol so we know the datatype
         */
        void onSymbolChanged(unsigned sym, Daedalus::EParOp op);

        /**
         * Callbacl for when an actual instance was created/removed
         * @param handle Handle to the created instance
         * @param instanceClass Class of the created instance
         */
        void onInstanceCreated(ZMemory::BigHandle handle, Daedalus::EInstanceClass instanceClass);
        void onInstanceRemoved(ZMemory::BigHandle handle, Daedalus::EInstanceClass instanceClass);

        /**
         * Engine and world this is running on
         */
        Engine::NetEngine& m_Engine;
        World::WorldInstance& m_World;


    };
}


