#pragma once
#include <string>
#include <handle/HandleDef.h>
#include <engine/World.h>

namespace Logic
{
    /**
     * Program based AI-states
     */
    enum class EPrgStates
    {
        NPC_PRGAISTATE_INVALID = -1,
        NPC_PRGAISTATE_ANSWER = 0,
        NPC_PRGAISTATE_DEAD,
        NPC_PRGAISTATE_UNCONSCIOUS,
        NPC_PRGAISTATE_FADEAWAY,
        NPC_PRGAISTATE_FOLLOW,
        NUM_PRGNPC_AISTATE
    };

	/**
	 * Structure describing an NPC-state
	 */
	struct NpcAIState 
	{
		enum class EPhase { Uninitialized = -1, Loop = 0, End = 1, Interrupt = 2 };

		NpcAIState()
		{
			symIndex = 0;
			symLoop = 0;
			symEnd = 0;
			phase = EPhase::Uninitialized;
			valid = false;
			stateTime = 0.0f;
            prgState = EPrgStates::NPC_PRGAISTATE_INVALID;
			isRoutineState = false;
		}

		// Index of the start-function symbol
		size_t			symIndex;

		// Index of the loop-function symbol
		size_t			symLoop;

		// Index of the end-function symbol
		size_t			symEnd;
		// -1 = not yet initialized, 0 = _LOOP, 1 = _END, 2 = _INTERRUPT
		EPhase			phase;

		// Whether the symbols are valid
		bool			valid;

		// Name of the state: ZS_name
		std::string		name;

		// How long this is already running
		float			stateTime;

		// User defined index to choose instead of calling a script-function
        EPrgStates		prgState;

		// Whether this is a state from a daily routine
		bool			isRoutineState;
	};

	class NpcScriptState
	{
	public:

		NpcScriptState(World::WorldInstance& world, Handle::EntityHandle hostVob);
		~NpcScriptState();

		/**
		 * @param symIdx Symbol-index of the state-main function
		 * @param endOldState Whether to end the currently active state as soon as possible
		 * @param timeBehaviour How this function should be executed
		 * @param isRoutineState Whether this belongs to the daily routine of the npc
		 * @return True, if the state was successfully activated. False if not possible or wanted for some reason.
		 */
		bool startAIState(size_t symIdx, bool endOldState, bool isRoutineState = false);
		bool startAIState(const std::string& name, bool endOldState, bool isRoutineState = false);

		/**
		 * @return Index of the last states main-function
		 */
		size_t getLastState() { return m_LastStateSymIndex; }

        /**
         * Checks whether the PLAYER going to the given state would be valid at this time
         * @param state State to check
         * @return Whether we could go to the input state
         */
        bool canPlayerUseAIState(const NpcAIState& state);

        /**
         * Performs the actions needed for the current frame and the current state. Advances to the next one, if needed
         * @param deltaTime time since last frame
         * @return Whether the state could be processed. This may be false when states are not activated for this NPC.
         */
        bool doAIState(float deltaTime);

        /**
         * Starts the routine-state set for this NPC
         * @return Whether the state could be started
         */
        bool startRoutineState();
	protected:

		/**
		 * Currently executed AI-state
		 */
		NpcAIState m_CurrentState;

		/**
		 * Next AI-state in queue
		 */
		NpcAIState m_NextState;

		/**
		 * Index of the last states main-function
		 */
		size_t m_LastStateSymIndex;

		/**
		 * World this resides in
		 */
		World::WorldInstance& m_World;

        /**
         * Vob this belongs to
         */
        Handle::EntityHandle m_HostVob;
	};
}