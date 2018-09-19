#include "MusicController.h"
#include <ZenLib/zenload/zTypes.h>
#include <audio/AudioWorld.h>
#include <components/Vob.h>
#include <components/VobClasses.h>
#include <debugdraw/debugdraw.h>
#include <engine/BaseEngine.h>
#include <engine/World.h>
#include <logic/ScriptEngine.h>
#include <logic/PlayerController.h>

using namespace Logic;

const std::array<const std::string, 6> MusicController::m_instanceSuffixes =
{
    "_DAY_STD",
    "_DAY_THR",
    "_DAY_FGT",
    "_NGT_STD",
    "_NGT_THR",
    "_NGT_FGT",
};

bool MusicController::m_debugDraw = false;
bool MusicController::m_playingMusic = false;

std::string MusicController::m_defaultZone = "DEF";

MusicController::MusicController(World::WorldInstance& world, Handle::EntityHandle entity)
    : Controller(world, entity)
    , m_isPlaying(false)
{
}

void MusicController::importObject(const json& j)
{
    Controller::importObject(j);
}

void MusicController::exportPart(json& j)
{
    Controller::exportPart(j);
}

void MusicController::initFromVobDescriptor(const ZenLoad::zCVobData& vob)
{
    assert(vob.vobType == ZenLoad::zCVobData::VT_oCZoneMusic);

    // The loaded bounding box is expressed in centimeters, while the engine uses meters
    // so we need to scale it by a factor of 100
    m_bbox[0] = vob.bbox[0] * 0.01;
    m_bbox[1] = vob.bbox[1] * 0.01;

    m_zoneName = vob.vobName;
    m_instancePrefix = Utils::uppered(vob.vobName.substr(vob.vobName.find('_') + 1));

    LogInfo() << "Music: controller created for " << m_zoneName;
}

void MusicController::playZone(World::WorldInstance& world, const std::string& prefix, EMusicTime time, EMusicType type)
{
    auto& audioWorld = world.getEngine()->getAudioWorld();
    if (!audioWorld.playMusicTheme(prefix + m_instanceSuffixes[time + type]))
    {
        if (!audioWorld.playMusicTheme(prefix + m_instanceSuffixes[MT_Day + type]))
        {
            m_playingMusic = audioWorld.playMusicTheme(prefix + m_instanceSuffixes[MT_Day + MT_Std]) || m_playingMusic;
        }
        else
        {
            m_playingMusic = true;
        }
    }
    else
    {
        m_playingMusic = true;
    }
}

void MusicController::onUpdate(float deltaTime)
{
    if (m_debugDraw)
    {
        Aabb box = {m_bbox[0].x, m_bbox[0].y, m_bbox[0].z, m_bbox[1].x, m_bbox[1].y, m_bbox[1].z};
        ddPush();
        ddSetColor(0xFF0000FF);
        ddDraw(box);
        ddPop();
    }

    EMusicTime time = m_World.getEngine()->getGameClock().isDaytime() ? MT_Day : MT_Ngt;
    const auto& audioWorld = m_World.getEngine()->getAudioWorld();
    const auto& currentTheme = audioWorld.currentMusicTheme();

    // We need to play a new segment in either one of two scenarios:
    // if the character has just entered the zone or
    // if the character was already in the zone but the time of day
    // changed.
    // FIXME: It'll also be needed to check if the character is
    // threatened or fighting to play the correct music.
    if (((!m_isPlaying || currentTheme.find(m_instancePrefix) != 0) && isInBoundingBox()) || (m_isPlaying && m_currentTime != time))
    {
        m_isPlaying = true;

        playZone(m_World, m_instancePrefix, time, MT_Std);
        m_currentTime = time;

        LogInfo() << "Music: entering " << m_zoneName;
    }
    else if (m_isPlaying && !isInBoundingBox())
    {
        m_isPlaying = false;

        playZone(m_World, m_defaultZone, time, MT_Std);
        m_currentTime = time;

        LogInfo() << "Music: exiting " << m_zoneName;
    }
}

bool MusicController::isInBoundingBox()
{
    // Get player's position
    VobTypes::NpcVobInformation player = VobTypes::asNpcVob(m_World, m_World.getScriptEngine().getPlayerEntity());
    Math::float3 pos{};

    if (player.isValid() && !player.playerController->getUsedMob().isValid())
    {
        pos = player.position->m_WorldMatrix.Translation();
    }

    return (pos.x >= m_bbox[0].x && pos.x < m_bbox[1].x &&
            pos.y >= m_bbox[0].y && pos.y < m_bbox[1].y &&
            pos.z >= m_bbox[0].z && pos.z < m_bbox[1].z);
}

void MusicController::playDefaultMusic(World::WorldInstance& world)
{
    EMusicTime time = world.getEngine()->getGameClock().isDaytime() ? MT_Day : MT_Ngt;
    playZone(world, m_defaultZone, time, MT_Std);
}
