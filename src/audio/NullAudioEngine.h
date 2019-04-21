#pragma once

#include "AudioEngine.h"
#include <exception>

namespace Audio
{
    class NullAudioSound;
    class NullAudioEngine final
        : public AudioEngine
    {
    public:
        NullAudioEngine() = default;
        ~NullAudioEngine() = default;

        void gain(float g) override
        {
            if(g < 0.0f) throw std::range_error("g");
        }

        void position(const Math::float3& p) override {}

        void velocity(const Math::float3& v) override {}

        void orientation(const Orientation& o) override {}

        SoundPtr createSound(const std::int16_t* buf, std::size_t len, Format, std::size_t) override;

        SoundPtr createSound(SoundStream, Format, std::size_t) override;
    };
}