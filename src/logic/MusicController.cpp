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

bool MusicController::m_debugDraw = false;

MusicController::MusicController(World::WorldInstance& world, Handle::EntityHandle entity)
    : Controller(world, entity)
{ }

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
}
