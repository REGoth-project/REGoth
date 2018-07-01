#include "Externals_Wld.h"
#include <components/VobClasses.h>
#include <daedalus/DaedalusStdlib.h>
#include <daedalus/DaedalusVM.h>
#include <debugdraw/debugdraw.h>
#include <engine/GameEngine.h>
#include <logic/DialogManager.h>
#include <logic/PlayerController.h>
#include <logic/ScriptEngine.h>
#include <logic/visuals/ModelVisual.h>
#include <ui/PrintScreenMessages.h>
#include <utils/logger.h>

using namespace Logic;

void ScriptExternals::registerEngineExternals_wld(World::WorldInstance& world,
                                                  Daedalus::DaedalusVM* vm,
                                                  bool verbose)
{
    Engine::BaseEngine* engine = world.getEngine();
    World::WorldInstance* pWorld = &world;
    using Daedalus::GameState::ItemHandle;
    using Daedalus::GameState::NpcHandle;

    auto isSymInstanceValid = [vm](size_t instance) {
        return vm->getDATFile().getSymbolByIndex(instance).instanceDataHandle.isValid();
    };

    auto getNPCByInstance = [=](size_t instance) {
        return pWorld->getScriptEngine().findNPCVobFromScriptInstance(instance);
    };

    auto getItemByInstance = [=](size_t instance) {
        return pWorld->getScriptEngine().findItemVobFromScriptInstance(instance);
    };

    vm->registerExternalFunction("wld_detectnpc", [=](Daedalus::DaedalusVM& vm) {
        int32_t guild = vm.popDataValue();
        int32_t aiState = vm.popDataValue();
        int32_t instance = vm.popDataValue();
        uint32_t self = vm.popVar();

        // TODO: Find using guild, etc

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            const std::set<Handle::EntityHandle>& worldNPCs = pWorld->getScriptEngine().getWorldNPCs();

            // Find the nearest NPC with the given criteria
            Math::float3 center = npc.position->m_WorldMatrix.Translation();

            //ddDrawAxis(center.x, center.y + 2, center.z, 2.0f);

            float nearest = FLT_MAX;
            Handle::EntityHandle nearestEnt;
            for (Handle::EntityHandle e : worldNPCs)
            {
                // Check position first (faster)
                Math::float3 position = pWorld->getEntity<Components::PositionComponent>(e).m_WorldMatrix.Translation();

                float dist = (center - position).lengthSquared();
                if (dist < nearest)
                {
                    if (e == npc.entity)
                        continue;

                    VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(*pWorld, e);
                    Daedalus::GEngineClasses::C_Npc& scriptInstance = VobTypes::getScriptObject(vob);

                    if (instance >= 0 && scriptInstance.instanceSymbol != static_cast<size_t>(instance)) continue;
                    if (guild >= 0 && scriptInstance.guild != guild) continue;
                    if (aiState >= 0 && vob.playerController->getAIStateMachine().isInState((size_t)aiState)) continue;

                    nearestEnt = e;
                    nearest = dist;
                }
            }

            // If found, put it into other
            if (nearestEnt.isValid())
            {
                VobTypes::NpcVobInformation vob = VobTypes::asNpcVob(*pWorld, nearestEnt);

                pWorld->getScriptEngine().setInstanceNPC("other", VobTypes::getScriptHandle(vob));
            }

            vm.setReturn(nearestEnt.isValid() ? 1 : 0);
        }
        else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("wld_istime", [=](Daedalus::DaedalusVM& vm) {
        int32_t min2 = vm.popDataValue();
        int32_t hour2 = vm.popDataValue();
        int32_t min1 = vm.popDataValue();
        int32_t hour1 = vm.popDataValue();

        // script sometimes uses negative values
        hour1 = (hour1 + 24) % 24;
        hour2 = (hour2 + 24) % 24;

        auto& clock = pWorld->getEngine()->getGameClock();
        double timeOfDayNow = clock.getTimeOfDay();
        double timeOfDay1 = Engine::GameClock::hmToDayTime(hour1, min1);
        double timeOfDay2 = Engine::GameClock::hmToDayTime(hour2, min2);

        const double minuteOfDayFraction = 1.0f / (24 * 60);
        if (timeOfDay1 != timeOfDay2)
            timeOfDay2 -= minuteOfDayFraction;  // Gothic subtracts one minute from the second time here to counter issues happening with overlapping times for Daily Routines.

        bool inside;

        if (hour1 == hour2 && min1 == min2)
        {
            // Scripts seem to expect this to return false. The original does this implicitly.
            inside = false;
        }
        else if (timeOfDay2 < timeOfDay1)
        {
            // Case: Interval contains midnight
            inside = timeOfDayNow >= timeOfDay1 || timeOfDayNow <= timeOfDay2;
        }
        else
        {
            // Case: Not wrapping around
            inside = timeOfDay1 <= timeOfDayNow && timeOfDayNow <= timeOfDay2;
        }
        vm.setReturn(inside);
    });

    vm->registerExternalFunction("wld_isfpavailable", [=](Daedalus::DaedalusVM& vm) {
        std::string fpname = vm.popString(true);
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            // Find closest fp
            std::vector<Handle::EntityHandle> fp = pWorld->getFreepointsInRange(npc.position->m_WorldMatrix.Translation(), 20.0f, fpname, true);

            vm.setReturn(!fp.empty());
        }
        else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("wld_isnextfpavailable", [=](Daedalus::DaedalusVM& vm) {
        std::string fpname = vm.popString(true);
        int32_t self = vm.popVar();

        VobTypes::NpcVobInformation npc = getNPCByInstance(self);

        if (npc.isValid())
        {
            // Find closest fp
            std::vector<Handle::EntityHandle> fp = pWorld->getFreepointsInRange(npc.position->m_WorldMatrix.Translation(), 20.0f, fpname, true, npc.entity);

            vm.setReturn(!fp.empty());
        }
        else
        {
            vm.setReturn(0);
        }
    });

    vm->registerExternalFunction("wld_insertnpc", [=](Daedalus::DaedalusVM& vm) {
        std::string spawnpoint = vm.popString();
        uint32_t npcinstance = vm.popDataValue();

        if (spawnpoint == "")
            return;

        if (!World::Waynet::waypointExists(pWorld->getWaynet(), spawnpoint) && !pWorld->doesFreepointExist(spawnpoint))
        {
            LogWarn() << "Invalid location: " << spawnpoint;
            return;
        }

        vm.getGameState().insertNPC(npcinstance, spawnpoint);
    });

    vm->registerExternalFunction("wld_insertitem", [=](Daedalus::DaedalusVM& vm) {
        std::string spawnpoint = vm.popString(true);
        uint32_t iteminstance = vm.popDataValue();

        // Create object
        Handle::EntityHandle e = VobTypes::initItemFromScript(*pWorld, iteminstance);

        // Position the vob
        Vob::VobInformation vob = Vob::asVob(*pWorld, e);

        // Try vobs first
        Handle::EntityHandle spawnEnt = pWorld->getVobEntityByName(spawnpoint);

        Math::float3 position;
        if (spawnEnt.isValid())
        {
            // Get position from object
            Components::PositionComponent& pos = pWorld->getEntity<Components::PositionComponent>(spawnEnt);
            position = pos.m_WorldMatrix.Translation();
        }
        else
        {
            // Then try waypoints
            World::Waynet::WaypointIndex wp = World::Waynet::getWaypointIndex(pWorld->getWaynet(), spawnpoint);

            if (wp == World::Waynet::INVALID_WAYPOINT)
            {
                LogWarn() << "Invalid location: " << spawnpoint;
                return;
            }

            position = pWorld->getWaynet().waypoints[wp].position;
        }

        // Move item to right place
        Vob::setPosition(vob, position);
    });

    vm->registerExternalFunction("wld_getday", [=](Daedalus::DaedalusVM& vm) {
        vm.setReturn(pWorld->getEngine()->getGameClock().getDay());
    });

    vm->registerExternalFunction("wld_getguildattitude", [=](Daedalus::DaedalusVM& vm) {
        int32_t victimGuild = vm.popDataValue();
        int32_t aggressorGuild = vm.popDataValue();
        const uint32_t numGuilds = 16;
        // aggressorGuild is the row index, victimGuild is the column index
        // TODO use copy of GIL_ATTITUDES Matrix instead
        auto attitude = vm.getDATFile().getSymbolByName("GIL_ATTITUDES").getInt(aggressorGuild * numGuilds + victimGuild);
        vm.setReturn(attitude);
    });
}
