//
// Created by desktop on 19.06.17.
//

#include <ZenLib/zenload/zTypes.h>
#include "SoundController.h"
#include <engine/World.h>
#include <engine/BaseEngine.h>

using namespace Logic;

SoundController::SoundController(World::WorldInstance& world, Handle::EntityHandle entity)
        : Controller(world, entity)
{

}

void SoundController::importObject(const json& j)
{
    Controller::importObject(j);
}

void SoundController::exportPart(json& j)
{
    Controller::exportPart(j);
}

void SoundController::playSound(const std::string& sound)
{
    m_PlayedSound = m_World.getAudioWorld().playSound(sound, getEntityTransform().Translation());

    // See when we need to play this sound the next time
    float totalSeconds = (float)m_World.getEngine()->getGameClock().getTotalSecondsRealtime();
    float offset = m_SoundPlayDelay + bx::flerp(-m_SoundDelayRandomness, m_SoundDelayRandomness, Utils::frand());

    m_SoundTimePlayNext = totalSeconds + offset;
}

void SoundController::initFromVobDescriptor(const ZenLoad::zCVobData& vob)
{
    m_SoundFile = vob.zCVobSound.sndName;
    m_SoundPlayDelay = vob.zCVobSound.sndRandDelay;
    m_SoundDelayRandomness = vob.zCVobSound.sndRandDelayVar;
}

void SoundController::onUpdate(float deltaTime)
{
    // Loop this sound after some time
    float totalSeconds = (float)m_World.getEngine()->getGameClock().getTotalSecondsRealtime();

    if(totalSeconds >= m_SoundTimePlayNext && !m_World.getAudioWorld().soundIsPlaying(m_PlayedSound))
    {
        playSound(m_SoundFile);
    }
}
