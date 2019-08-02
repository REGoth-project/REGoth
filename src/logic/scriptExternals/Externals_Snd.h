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
         * Registers our externals
         */
        void registerEngineExternals_snd(World::WorldInstance& world,
                                         Daedalus::DaedalusVM* vm,
                                         bool verbose = false);
    }
}