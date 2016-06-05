#pragma once

namespace Daedalus
{
    class DaedalusVM;
}

namespace Logic
{
    namespace ScriptExternals
    {
        /**
         * Registers stubs for most known script externals
         */
        void registerStubs(Daedalus::DaedalusVM& vm, bool verbose = false);
    }
}