#pragma once

namespace Engine
{
    class BaseEngine;
}
namespace World
{
    class WorldInstance;
};

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
        void registerEngineExternals(World::WorldInstance& world, Daedalus::DaedalusVM* vm, bool verbose = false);
    }
}
