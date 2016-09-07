#pragma once
#include <string>
#include <daedalus/DaedalusGameState.h>
#include <handle/HandleDef.h>
#include <set>
#include <daedalus/DaedalusVM.h>
#include <math/mathlib.h>

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
        int32_t runFunction(size_t addr);
        int32_t runFunctionBySymIndex(size_t symIdx);

        /**
         * Returns the current script-gamestate
         */
        Daedalus::GameState::DaedalusGameState& getGameState();

        /**
         * Returns the underlaying VM for some internal work
         */
        Daedalus::DaedalusVM& getVM(){return *m_pVM; }

        /**
         * Returns the symbol-index of the given symbol-name
         * @return Symbol-index, -1 of not found
         */
        size_t getSymbolIndexByName(const std::string& name);

        /**
         * @return The entity of the NPC the player is currently playing as
         */
        Handle::EntityHandle getPlayerEntity(){ return m_PlayerEntity; }

        /**
         * Returns a list of all npcs found inside the given sphere
         * @param center Center of the search-sphere
         * @param radius Radius of the search-sphere
         * @return List of found NPCs
         */
        std::set<Handle::EntityHandle> getNPCsInRadius(const Math::float3& center, float radius);

        /**
         * @return List of all registered NPCs in the world
         */
        const std::set<Handle::EntityHandle>& getWorldNPCs(){ return m_WorldNPCs; }
    protected:

        /**
         * Called when an npc got inserted into the world
         */
        void onNPCInserted(Daedalus::GameState::NpcHandle npc, const std::string& spawnpoint);

        /**
         * Called after the NPC got inserted into the world. At this point, it is fully initialized and can be used.
         */
        void onNPCInitialized(Daedalus::GameState::NpcHandle npc);

        /**
         * Called when an item got inserted into some NPCs inventory
         */
        void onItemInserted(Daedalus::GameState::ItemHandle item, Daedalus::GameState::NpcHandle npc);

        /**
         * Called when a log-entry was inserted
         */
        void onLogEntryAdded(const std::string& topic, const std::string& entry);

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

        /**
         * NPC-Entity of the player
         */
        Handle::EntityHandle m_PlayerEntity;
    };
}