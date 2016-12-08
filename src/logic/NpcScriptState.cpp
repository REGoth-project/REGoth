#include "NpcScriptState.h"
#include <components/Vob.h>
#include <ZenLib/utils/logger.h>
#include <components/VobClasses.h>
#include "PlayerController.h"
#include <logic/visuals/ModelVisual.h>

using namespace Logic;

/**
 * Valid script-states an NPC can be in
 */
static const std::string s_EnabledPlayerStates[] = {
        "ZS_ASSESSMAGIC",
        "ZS_ASSESSSTOPMAGIC",
        "ZS_MAGICFREEZE",
        "ZS_WHIRLWIND",
        "ZS_SHORTZAPPED",
        "ZS_ZAPPED",
        "ZS_PYRO",
        "ZS_MAGICSLEEP"
};

NpcScriptState::NpcScriptState(World::WorldInstance& world, Handle::EntityHandle hostVob) :
    m_World(world),
    m_HostVob(hostVob)
{
    m_Routine.startNewRoutine = true;
    m_Routine.hasRoutine = false;
    m_Routine.routineActiveIdx = 0;
}

NpcScriptState::~NpcScriptState()
{

}


bool Logic::NpcScriptState::startAIState(size_t symIdx, bool endOldState, bool isRoutineState)
{
    VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(m_World, m_HostVob);
    ScriptEngine& s = m_World.getScriptEngine();
    Daedalus::DATFile& dat = s.getVM().getDATFile();

    // Save script variables
    m_StateOther = s.getNPCFromSymbol("other");
    m_StateVictim = s.getNPCFromSymbol("victim");
    m_StateItem = s.getItemFromSymbol("item");

    m_NextState.name = dat.getSymbolByIndex(symIdx).name;

    //LogInfo() << "AISTATE-START: " << m_NextState.name << " on NPC: " << VobTypes::getScriptObject(vob).name[0] << " (WP: " << VobTypes::getScriptObject(vob).wp << ")";

    // Check if this is just a usual action (ZS = German "Zustand" = State, B = German "Befehl" = Instruction)
    if(m_NextState.name.substr(0, 3) != "ZS_")
    {
        // Disable routine-flag for this, since scripts could let the npc assess something
        bool oldIsRoutineState = m_CurrentState.isRoutineState;
        m_CurrentState.isRoutineState = isRoutineState;

        // Just call the function
        s.prepareRunFunction();
        s.setInstance("self", VobTypes::getScriptObject(vob).instanceSymbol);
        s.runFunctionBySymIndex(symIdx);

        m_CurrentState.isRoutineState = oldIsRoutineState;
        return true;
    }

    m_NextState.isRoutineState = isRoutineState;

    m_NextState.symIndex = symIdx;
    m_NextState.symEnd = 0;
    m_NextState.symLoop = 0;

    if(dat.hasSymbolName(m_NextState.name + "_END"))
        m_NextState.symEnd = dat.getSymbolIndexByName(m_NextState.name + "_END");

    if(dat.hasSymbolName(m_NextState.name + "_LOOP"))
        m_NextState.symLoop = dat.getSymbolIndexByName(m_NextState.name + "_LOOP");


    m_NextState.valid = true;

    if(endOldState)
    {
        m_CurrentState.phase = NpcAIState::EPhase::End;
    } else
    {
        // Just interrupt the state
        m_CurrentState.phase = NpcAIState::EPhase::Interrupt;

        // If this is not a player, or the player is valid to perform this state...
        if(!vob.playerController->isPlayerControlled() || canPlayerUseAIState(m_NextState))
        {
            vob.playerController->getModelVisual()->stopAnimations();
            vob.playerController->interrupt();
            vob.playerController->getEM().clear();
        }
    }

	return doAIState(0.0);
}

bool Logic::NpcScriptState::startAIState(const std::string & name, bool endOldState, bool isRoutineState)
{
    // Get script symbol index
    size_t idx = m_World.getScriptEngine().getVM().getDATFile().getSymbolIndexByName(name);

	return startAIState(idx, endOldState, isRoutineState);
}

bool NpcScriptState::canPlayerUseAIState(const NpcAIState& state)
{
    if(state.valid)
    {
        if(state.prgState != EPrgStates::NPC_PRGAISTATE_INVALID)
        {
            // States that are allowed for every NPC at any time
            return (state.prgState == EPrgStates::NPC_PRGAISTATE_DEAD
                    || state.prgState == EPrgStates::NPC_PRGAISTATE_UNCONSCIOUS);
        } else
        {
            for(const std::string& s : s_EnabledPlayerStates)
            {
                if(state.name == s)
                    return true;
            }
        }
    }

    return false;
}

bool NpcScriptState::doAIState(float deltaTime)
{
    VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(m_World, m_HostVob);
    ScriptEngine& s = m_World.getScriptEngine();
    Daedalus::DATFile& dat = s.getVM().getDATFile();

    // Increase time this state is already running
    if(m_CurrentState.valid && m_CurrentState.phase == NpcAIState::EPhase::Loop)
        m_CurrentState.stateTime += deltaTime;

    std::string name = vob.playerController->getScriptInstance().name[0];

    if(m_Routine.hasRoutine && isInRoutine())
    {
        int h, m;
        m_World.getSky().getTimeOfDay(h, m);

        if(!m_Routine.routine[m_Routine.routineActiveIdx].timeInRange(h, m))
        {
            // Find next
            size_t i=0;
            for(RoutineEntry& e : m_Routine.routine)
            {
                if(e.timeInRange(h, m) && i != m_Routine.routineActiveIdx)
                {
                    m_Routine.routineActiveIdx = i;
                    m_Routine.startNewRoutine = true;
                    break;
                }

                i++;
            }
        }
    }

    // Only do states if we do not have messages pending
    if(vob.playerController->getEM().isEmpty())
    {
        // Check for routine first
        if(m_Routine.startNewRoutine
           && m_Routine.hasRoutine
           && isInRoutine()
           && !vob.playerController->isPlayerControlled())
            startRoutineState();

        if(!m_CurrentState.valid)
        {
            // Can we move to the next state?
            if(m_NextState.valid)
            {
                // Remember the last state we were in
                m_LastStateSymIndex = m_CurrentState.symIndex;

                // Move to next state
                m_CurrentState = m_NextState;
                m_CurrentState.stateTime = 0.0f;

                m_NextState.valid = false;
            } else
            {
                //LogInfo() << "No active state for NPC: " << VobTypes::getScriptObject(vob).name[0];
                startRoutineState();
            }
        }

        // If this is the player, only allow states the player is allowed to have
        if(vob.playerController->isPlayerControlled() && !canPlayerUseAIState(m_CurrentState))
            return false;

        if(m_CurrentState.valid)
        {
            // Prepare state function call
            auto& inst = VobTypes::getScriptObject(vob);
            s.setInstanceNPC("self", VobTypes::getScriptHandle(vob));

            // These are set by the game, but seem to be always 0
            s.setInstanceNPC("OTHER", m_StateOther);
            s.setInstanceNPC("VICTIM", m_StateVictim);
            s.setInstanceItem("ITEM", m_StateItem);

            if(m_CurrentState.phase == NpcAIState::EPhase::Uninitialized)
            {
                // TODO: Set perception-time to 5000

                //LogInfo() << "AISTATE-INIT: " << m_CurrentState.name << " on NPC: " << VobTypes::getScriptObject(vob).name[0] << " (WP: " << VobTypes::getScriptObject(vob).wp << ")";

                if(m_CurrentState.symIndex > 0)
                {
                    // Call setup-function
                    s.prepareRunFunction();
                    s.runFunctionBySymIndex(m_CurrentState.symIndex);
                }

                // Now do the looping function every frame
                m_CurrentState.phase = NpcAIState::EPhase::Loop;
            }else if(m_CurrentState.phase == NpcAIState::EPhase::Loop)
            {
                bool end = true;

                //LogInfo() << "AISTATE-LOOP: " << m_CurrentState.name << " on NPC: " << VobTypes::getScriptObject(vob).name[0] << " (WP: " << VobTypes::getScriptObject(vob).wp << ")";

                // Call looping-function
                if(m_CurrentState.symLoop > 0)
                {
                    s.prepareRunFunction();
                    end = s.runFunctionBySymIndex(m_CurrentState.symLoop) != 0;
                }

                // Run a program based state
                if(m_CurrentState.prgState != EPrgStates::NPC_PRGAISTATE_INVALID)
                {
                    // Only CheckUnconscious() is called here. There is also a state for following, but it doesn't do anything here
                    // TODO: call CheckUnconscious
                }

                // Check if we're done and remove the state in the next frame
                if(end)
                    m_CurrentState.phase = NpcAIState::EPhase::End;

            }else if(m_CurrentState.phase == NpcAIState::EPhase::End)
            {
                //LogInfo() << "AISTATE-END: " << m_CurrentState.name << " on NPC: " << VobTypes::getScriptObject(vob).name[0] << " (WP: " << VobTypes::getScriptObject(vob).wp << ")";

                // Call end-function
                if(m_CurrentState.symEnd > 0)
                {
                    s.prepareRunFunction();
                    s.runFunctionBySymIndex(m_CurrentState.symEnd);
                }

                // Invalidate the state and get it ready for the next one
                m_CurrentState.phase = NpcAIState::EPhase::Interrupt;
                m_CurrentState.valid = false;
            }
        }
    }

    return true;
}

bool NpcScriptState::startRoutineState(bool force)
{
    VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, m_HostVob);

    // Just say "yes" for the player, as he never has any routine
    if(npc.playerController->isPlayerControlled())
        return true;

    // Turn off, so we don't recurse
    m_Routine.startNewRoutine = false;

    // Start new state
    bool res = activateRoutineState(force);
    m_Routine.startNewRoutine = !res;

    return res;
}

bool NpcScriptState::isInRoutine()
{
    return (m_CurrentState.valid && m_CurrentState.isRoutineState)
            || (!m_CurrentState.valid && !m_NextState.valid);
}

bool NpcScriptState::activateRoutineState(bool force)
{
    VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, m_HostVob);

    // Player can't have routines
    if(npc.playerController->isPlayerControlled())
        return false;

    // Check for death, etc
    if(!npc.playerController->isNpcReady())
        return false;

    if(m_Routine.hasRoutine)
    {
        // No routine present?
        if(m_Routine.routine.empty())
            return false;

        if(!force)
        {
            if(!npc.playerController->getEM().isEmpty())
                return false; // EM not empty, don't start routine yet!

            // Check if the routine is already running
            if(getCurrentStateSym() == m_Routine.routine[m_Routine.routineActiveIdx].symFunc)
                return true;

            if(!isInRoutine())
                return false; // Currently in other state...
        }

        // Set current waypoint of script instance
        npc.playerController->getScriptInstance().wp = m_Routine.routine[m_Routine.routineActiveIdx].waypoint;

        return startAIState(m_Routine.routine[m_Routine.routineActiveIdx].symFunc, true, true);
    }else if(isNpcStateDriven())
    {
        //return false;
        if(force)
        {
            // Make it look like we don't have anything running now
            m_NextState.valid = false;
            m_CurrentState.valid = false;
        }

        // Don't make a new state if we already have something running
        if(m_CurrentState.valid || m_NextState.valid)
            return false;

        return startAIState(VobTypes::getScriptObject(npc).start_aistate, true);
    }

    return false;
}

void NpcScriptState::insertRoutine(int hoursStart, int minutesStart, int hoursEnd, int minutesEnd, size_t symFunc,
                                   const std::string& waypoint)
{
    // FIXME: HACK, let the npc teleport to the first entry of the routine
    if(!m_Routine.hasRoutine)
    {
        VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, m_HostVob);

		World::Waynet::WaypointIndex wp = World::Waynet::getWaypointIndex(m_World.getWaynet(), waypoint);

		if(wp != World::Waynet::INVALID_WAYPOINT)
			npc.playerController->teleportToWaypoint(wp);
    }

    m_Routine.hasRoutine = true; // At least one routine-target present

    // Make new routine entry
    RoutineEntry entry;
    entry.hoursStart = hoursStart;
    entry.hoursEnd = hoursEnd;
    entry.minutesStart = minutesStart;
    entry.minutesEnd = minutesEnd;
    entry.symFunc = symFunc;
    entry.waypoint = waypoint;
    entry.isOverlay = false;

    m_Routine.routine.push_back(entry);


}

bool NpcScriptState::isNpcStateDriven()
{
    VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, m_HostVob);

    return VobTypes::getScriptObject(npc).start_aistate != 0;
}

size_t NpcScriptState::getCurrentStateSym()
{
    if(m_CurrentState.valid)
    {
        if(m_CurrentState.prgState != EPrgStates::NPC_PRGAISTATE_INVALID)
            return static_cast<size_t>(m_CurrentState.prgState);

        return m_CurrentState.symIndex;
    }

    return 0;
}

void NpcScriptState::reinitRoutine()
{
    VobTypes::NpcVobInformation npc = VobTypes::asNpcVob(m_World, m_HostVob);
    ScriptEngine& s = m_World.getScriptEngine();

    size_t newSymFn = VobTypes::getScriptObject(npc).daily_routine;

    // Clear old routine
    m_Routine.routine.clear();
    m_Routine.routineActiveIdx = 0;

    if(newSymFn != 0)
    {
        s.prepareRunFunction();
        s.runFunctionBySymIndex(newSymFn);

        LogInfo() << "Changed routine on "
                  << VobTypes::getScriptObject(npc).name[0]
                  << " to: "
                  << s.getVM().getDATFile().getSymbolByIndex(m_Routine.routine[0].symFunc).name;
    }
}

bool NpcScriptState::isInState(size_t stateMain)
{
    if (m_CurrentState.valid)
    {
        return m_CurrentState.symIndex == stateMain;
    } else if (m_NextState.valid)
    {
        return m_NextState.symIndex == stateMain;
    }

    return false;
}



void NpcScriptState::importState(NpcAIState& state, const json& j) const
{
    state.symIndex = j["symIndex"];
    state.symLoop = j["symLoop"];
    state.symEnd = j["symEnd"];
    state.phase = (NpcAIState::EPhase)((int)j["phase"]);
    state.valid = j["valid"];
    state.name = j["name"];
    state.stateTime = j["stateTime"];
    state.prgState = (EPrgStates)((int)j["prgState"]);
    state.isRoutineState = j["isRoutineState"];
}

void NpcScriptState::exportState(const NpcAIState& state, json& j) const
{
    j["symIndex"] = state.symIndex;
    j["symLoop"] = state.symLoop;
    j["symEnd"] = state.symEnd;
    j["phase"] = (int)state.phase;
    j["valid"] = state.valid;
    j["name"] = state.name;
    j["stateTime"] = state.stateTime;
    j["prgState"] = (int)state.prgState;
    j["isRoutineState"] = state.isRoutineState;
}

void NpcScriptState::exportScriptState(json& j)
{
    // Export current state
    exportState(m_CurrentState, j["currentState"]);

    // Export next state
    exportState(m_NextState, j["nextState"]);

    j["lastStateSymIndex"] = m_LastStateSymIndex;

    // TODO: Need other/victim/item?

    // Export routine
    j["routines"] = json::array();
    for(const RoutineEntry& r : m_Routine.routine)
    {
        json jr;
        jr["hoursStart"] =      r.hoursStart;
        jr["minutesStart"] =    r.minutesStart;
        jr["hoursEnd"] =        r.hoursEnd;
        jr["minutesEnd"] =      r.minutesEnd;
        jr["symFunc"] =         r.symFunc;
        jr["waypoint"] =        r.waypoint;
        jr["isOverlay"] =       r.isOverlay;

        j["routines"].push_back(jr);
    }
    j["activeRoutineIdx"] = m_Routine.routineActiveIdx;
}

void NpcScriptState::importScriptState(const json& j)
{
    importState(m_CurrentState, j["currentState"]);
    importState(m_NextState, j["nextState"]);

    m_LastStateSymIndex = j["lastStateSymIndex"];

    m_Routine.routine.clear();
    for(const json& jr : j["routines"])
    {
        RoutineEntry r;
        r.hoursStart = jr["hoursStart"];
        r.minutesStart = jr["minutesStart"];
        r.hoursEnd = jr["hoursEnd"];
        r.minutesEnd = jr["minutesEnd"];
        r.symFunc = jr["symFunc"];
        r.waypoint = jr["waypoint"];
        r.isOverlay = jr["isOverlay"];

        m_Routine.routine.push_back(r);
    }

    m_Routine.routineActiveIdx = j["activeRoutineIdx"];
    m_Routine.hasRoutine = !m_Routine.routine.empty();

    // Start routine
    //reinitRoutine();
}

