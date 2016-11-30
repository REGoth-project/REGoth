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
        ScriptEngine(World::WorldInstance& world, ScriptEngine&& other);
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
         * Frame-functions
         */
        void onFrameStart();
        void onFrameEnd();

        /**
         * Saves the state of the VM and prepares it for a call to runFunction.
         * You can push function arguments after this call.
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
        void setInstanceNPC(const std::string& target, Daedalus::GameState::NpcHandle npc);
        void setInstanceItem(const std::string& target, Daedalus::GameState::NpcHandle npc);

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
         * Checks whether the given symbol exists
         * @param name Symbol to check
         * @return Whether a symbol with the given name exists
         */
        bool hasSymbol(const std::string& name);

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

        /**
         * Looks up the handle currently stored inside the given symbol. If it doesn't hold the right type or nothing
         * at all, an invalid handle is returned
         * @param symName Symbol to look up
         * @return Handle to the instance. Invalid, if empty or wrong type.
         */
        Daedalus::GameState::NpcHandle getNPCFromSymbol(const std::string& symName);
        Daedalus::GameState::ItemHandle getItemFromSymbol(const std::string& symName);

        /**
         * (Un)Registers an item-instance currently sitting inside the world
         * @param e Entity of the item-instance
         */
        void registerItem(Handle::EntityHandle e);
        void unregisterItem(Handle::EntityHandle e);

        void registerMob(Handle::EntityHandle e);
        void unregisterMob(Handle::EntityHandle e);

        /**
         * Applies the given items effects on the given NPC or equips it. Does not delete the item or anything else.
         * @param item Item to apply the effects from
         * @param npc NPC to apply the effects to
         * @return false, if this item is not usable and can not be equipped
         */
        bool useItemOn(Daedalus::GameState::ItemHandle item, Handle::EntityHandle npc);

        /**
         * @return All items found inside the World
         */
        const std::set<Handle::EntityHandle>& getWorldItems(){ return m_WorldItems; }

        /**
         * @return All mobs found inside the World
         */
        const std::set<Handle::EntityHandle>& getWorldMobs(){ return m_WorldMobs; }

        /**
         * @return Profile data for this frame. (Time by function-symbol)
         */
        const std::map<size_t, double>& getProfilingData(){ return m_TimeByFunctionSymbol[m_ProfilingDataFrame]; };

        /**
         * Resets profiling data for the current frame
         */
        void resetProfilingData();

    protected:

        /**
         * Starts/stops profiling on the given function-symbol
         */
        void startProfiling(size_t fnSym);
        void stopProfiling(size_t fnSym);

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
        void onInventoryItemInserted(Daedalus::GameState::ItemHandle item, Daedalus::GameState::NpcHandle npc);

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
         * All NPCs/Items currently in the world
         */
        std::set<Handle::EntityHandle> m_WorldNPCs;
        std::set<Handle::EntityHandle> m_WorldItems;
        std::set<Handle::EntityHandle> m_WorldMobs;

        /**
         * NPC-Entity of the player
         */
        Handle::EntityHandle m_PlayerEntity;

        /**
         * Profiling
         */
        std::map<size_t, double> m_TimeByFunctionSymbol[10];
        std::stack<int64_t> m_TimeStartStack;
        int m_ProfilingDataFrame;
    };
}