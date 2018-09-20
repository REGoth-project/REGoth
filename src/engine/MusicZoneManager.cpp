#include "MusicZoneManager.h"

#include <components/Vob.h>
#include <components/VobClasses.h>
#include <engine/BaseEngine.h>
#include <logic/PlayerController.h>
#include <logic/ScriptEngine.h>

using namespace Engine;

static const std::array<const std::string, 6> instanceSuffixes =
{
    "_DAY_STD",
    "_DAY_THR",
    "_DAY_FGT",
    "_NGT_STD",
    "_NGT_THR",
    "_NGT_FGT",
};

MusicZoneManager::MusicZoneManager(World::WorldInstance& world)
    : m_world(world)
{
}

void MusicZoneManager::addZone(const ZenLoad::zCVobData& zone)
{
    assert(zone.vobType == ZenLoad::zCVobData::VT_oCZoneMusic);
    m_zones.emplace_back(zone);
}

static bool isInside(const Math::float3& vec, const ZMath::float3& min, const ZMath::float3& max)
{
    return (vec.x >= min.x && vec.x < max.x &&
            vec.y >= min.y && vec.y < max.y &&
            vec.z >= min.z && vec.z < max.z);
}

void MusicZoneManager::playTheme(const std::string& prefix, EMusicType type, EMusicTime time)
{
    auto& audioWorld = m_world.getEngine()->getAudioWorld();
    // First try the requested theme
    if (!audioWorld.playMusicTheme(prefix + instanceSuffixes[time + type]))
    {
        // If it wasn't found, try the daytime variation
        if (!audioWorld.playMusicTheme(prefix + instanceSuffixes[MT_Day + type]))
        {
            // Then try the standard daytime variation
            audioWorld.playMusicTheme(prefix + instanceSuffixes[MT_Day + MT_Std]);
        }
    }
}

void MusicZoneManager::onUpdate()
{
    // Get player's position
    const auto& playerEntity = m_world.getScriptEngine().getPlayerEntity();
    auto playerVob = VobTypes::asNpcVob(m_world, playerEntity);

    if (!playerVob.isValid() || playerVob.playerController->getUsedMob().isValid())
        return;

    const auto& playerPos = playerVob.position->m_WorldMatrix.Translation();

    const ZenLoad::zCVobData* currentZone = nullptr;
    for (const auto& zone : m_zones)
    {
        // BBoxes are expressed in centimeters, but REGoth uses meters
        if (isInside(playerPos, zone.bbox[0] * .01f, zone.bbox[1] * .01f))
        {
            if (!currentZone
                || currentZone->oCZoneMusic.priority < zone.oCZoneMusic.priority)
            {
                currentZone = &zone;
            }
        }
    }

    auto time = m_world.getEngine()->getGameClock().isDaytime() ? MT_Day : MT_Ngt;

    auto& audioWorld = m_world.getEngine()->getAudioWorld();

    if (!currentZone)
    {
        // If the player's not inside any zone, play the default theme
        playTheme(m_defaultZonePrefix, MT_Std, time);
    }
    else
    {
        auto zoneName = currentZone->vobName;
        auto instancePrefix = Utils::uppered(zoneName.substr(zoneName.find('_') + 1));

        // Try playing the zone's theme
        playTheme(instancePrefix, MT_Std, time);
    }
}