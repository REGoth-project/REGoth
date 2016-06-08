#pragma once

namespace Engine
{
    class BaseEngine;
}

namespace Daedalus
{
    class DaedalusVM;
}

namespace Logic
{
    namespace ScriptExternals
    {
        /**
         * Registers the standard-library
         */
        void registerStdLib(Daedalus::DaedalusVM& vm, bool verbose = false);

        /**
         * Registers our externals
         */
        void registerEngineExternals(Engine::BaseEngine* engine, Daedalus::DaedalusVM* vm, bool verbose = false);
    }
}
