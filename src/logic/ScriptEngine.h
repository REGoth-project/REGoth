#pragma once
#include <string>
#include <daedalus/DaedalusGameState.h>
#include <handle/HandleDef.h>
#include <set>

namespace Daedalus
{
    class DaedalusVM;
}

namespace World
{
    class WorldInstance;
}

namespace Logic
{
    class ScriptEngine
    {
    public:
        ScriptEngine(World::WorldInstance& world);
        virtual ~ScriptEngine();

        /**
         * Loads a compiled script data-file
         */
        bool loadDAT(const std::string& file);

        /**
         * Initializes for the world with the given name (startup_x, init_x)
         */
        void initForWorld(const std::string& world);

        /**
         * Saves the state of the VM and prepares it for a call to runFunction
         */
        void prepareRunFunction();

        /**
         * Pushes an int to the stack
         */
        void pushInt(int32_t v);
        void pushString(const std::string& str);
        void pushSymbol(size_t sym, uint32_t arrayIndex = 0);
        void pushSymbol(const std::string& sname);

        /**
         * Sets the instance of the given symbol
         */
        void setInstance(const std::string& target, const std::string& source);
        void setInstance(const std::string& target, size_t source);

        /**
         * Runs a complete function with the arguments given by pushing onto the stack
         * Note: Must be prepared first, using prepareRunFunction.
         * @param fname Symbol-name of the function to look up and call
         * @return value returned by the function
         */
        int32_t runFunction(const std::string& fname);

        /**
         * Returns the current script-gamestate
         */
        Daedalus::GameState::DaedalusGameState& getGameState();
    protected:

        /**
         * Called when an npc got inserted into the world
         */
        void onNPCInserted(Daedalus::GameState::NpcHandle npc, const std::string& spawnpoint);

        /**
         * Script-VM
         */
        Daedalus::DaedalusVM* m_pVM;

        /**
         * World this operates on
         */
        World::WorldInstance& m_World;

        /**
         * All NPCs currently in the world
         */
        std::set<Handle::EntityHandle> m_WorldNPCs;
    };
}