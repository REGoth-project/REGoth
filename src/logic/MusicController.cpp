#include "MusicController.h"
#include <ZenLib/zenload/zTypes.h>
#include <engine/BaseEngine.h>
#include <logic/ScriptEngine.h>
#include <engine/World.h>
#include <debugdraw/debugdraw.h>

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

MusicController::MusicController(World::WorldInstance& world, Handle::EntityHandle entity)
    : Controller(world, entity)
    , m_isPlaying(false) {}

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
    m_instancePrefix = vob.vobName.substr(vob.vobName.find('_') + 1);

    LogInfo() << "Music: controller created for " << m_zoneName;
}

void MusicController::onUpdate(float deltaTime)
{
    if (m_debugDraw)
    {
        Aabb box = { m_bbox[0].x, m_bbox[0].y, m_bbox[0].z, m_bbox[1].x, m_bbox[1].y, m_bbox[1].z };
        ddPush();
        ddSetColor(0xFF0000FF);
        ddDraw(box);
        ddPop();
    }

    if (!m_isPlaying && isInBoundingBox())
    {
        m_isPlaying = true;
        int index = MT_Day + MT_Std; // FIXME: Should use the correct index based on time and situation

        m_World.getAudioWorld().playMusicTheme(m_instancePrefix + m_instanceSuffixes[index]);

        LogInfo() << "Music: entering " << m_zoneName;
    }
    else if (m_isPlaying && !isInBoundingBox())
    {
        m_isPlaying = false;
        LogInfo() << "Music: exiting " << m_zoneName;
    }
}

bool MusicController::isInBoundingBox()
{
    Math::float3 cam = m_World.getCameraComp<Components::PositionComponent>().m_WorldMatrix.Translation();

    if (cam.x >= m_bbox[0].x && cam.x < m_bbox[1].x &&
        cam.y >= m_bbox[0].y && cam.y < m_bbox[1].y &&
        cam.z >= m_bbox[0].z && cam.z < m_bbox[1].z)
    {
        return true;
    }

    return false;
}
