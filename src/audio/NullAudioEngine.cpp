#include "NullAudioEngine.h"

using namespace Audio;

class NullSound : public Sound
{
public:
    bool isPlaying() const { return false; }
    bool isLooping() const { return false; }
    bool isPaused() const { return false; }

    void stop() {}
    void pause() {}
    void play() {}

    const Math::float3& getPosition() const { return Math::float3(0,0,0); }
    const Math::float3& getVelocity() const { return Math::float3(0,0,0); }
    float getPitch() const { return 1; }
    float getMaxDistance() const { return 0; }
    float getGain() const { return 1; }
    bool isRelative() const { return false; }

    void setPosition(const Math::float3& pos) {}
    void setVelocity(const Math::float3& vel) {}
    void setPitch(float pitch) {}
    void setMaxDistance(float dist) {}
    void setGain(float gain) {}
    void setRelative(bool isRelative) {}
    void setLooping(bool isLooping) {}
};

std::shared_ptr<Sound> NullAudioEngine::createSound(const std::vector<std::int16_t>& data, AudioFormat format, int samplingRate)
{
    return std::make_shared<NullSound>();
}

std::shared_ptr<Sound> NullAudioEngine::createSound(const std::vector<std::int8_t>& data, AudioFormat format, int samplingRate)
{
    return std::make_shared<NullSound>();
}

std::shared_ptr<Sound> NullAudioEngine::createSound(std::function<int(std::int16_t* buf, int len)> source, AudioFormat format, int samplingRate)
{
    return std::make_shared<NullSound>();
}

std::shared_ptr<Sound> NullAudioEngine::createSound(std::function<int(std::int8_t* buf, int len)> source, AudioFormat format, int samplingRate)
{
    return std::make_shared<NullSound>();
}