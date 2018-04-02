#include "MusicController.h"
#include <ZenLib/zenload/zTypes.h>
#include <engine/BaseEngine.h>
#include <logic/ScriptEngine.h>
#include <engine/World.h>

using namespace Logic;

MusicController::MusicController(World::WorldInstance& world, Handle::EntityHandle entity)
    : Controller(world, entity)
    , m_isPlaying(false)
{}

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
    if (!m_isPlaying && isInBoundingBox())
    {
        m_isPlaying = true;
        // Play segment
        LogInfo() << "Music: entering " << m_zoneName << " (" << m_instancePrefix << "_Day_Std)";
    }
    else if (m_isPlaying && !isInBoundingBox())
    {
        m_isPlaying = false;
        LogInfo() << "Music: exiting " << m_zoneName << " (" << m_instancePrefix << "_Day_Std)";
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
