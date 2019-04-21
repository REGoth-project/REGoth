#include "NullAudioEngine.h"

using namespace Audio;

namespace Audio {
class NullAudioSound final
    : public Sound
{
public:
    void pitch(float p) override {}
    void gain(float g) override
    {
        if(g < 0.0f) throw std::out_of_range("g");
    }
    void maxDistance(float md) override
    {
        if(md < 0.0f) throw std::out_of_range("md");
    }
    void position(const Math::float3& p) override {}
    void velocity(const Math::float3& v) override {}
    void direction(const Math::float3& d) override {}
    void relative(bool r) override {}
    void looping(bool l) override {}
    State state() override { return m_state; }
    void play() override { m_state = State::Playing; }
    void pause() override { m_state = State::Paused; }
    void stop() override { m_state = State::Stopped; }
private:
    State m_state;
};
}

SoundPtr NullAudioEngine::createSound(const std::int16_t*, std::size_t, Format, std::size_t)
{
    return std::make_shared<NullAudioSound>();
}

SoundPtr NullAudioEngine::createSound(SoundStream, Format, std::size_t)
{
    return std::make_shared<NullAudioSound>();
}