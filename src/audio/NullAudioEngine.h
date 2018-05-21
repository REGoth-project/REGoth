#pragma once

#include "AudioEngine.h"

namespace Audio
{
    class NullAudioEngine : public AudioEngine
    {
    public:
        NullAudioEngine() {}

        void setListenerPosition(const Math::float3& pos) {}
        void setListenerVelocity(const Math::float3& vel) {}
        void setListenerOrientation(const std::array<float, 6>& orient) {}
        void setListenerGain(float gain) {}

        std::shared_ptr<Sound> createSound(const std::vector<std::int16_t>& data, AudioFormat format, int samplingRate);
        std::shared_ptr<Sound> createSound(const std::vector<std::int8_t>& data, AudioFormat format, int samplingRate);
        std::shared_ptr<Sound> createSound(std::function<int(std::int16_t *buf, int len)> source, AudioFormat format, int samplingRate);
        std::shared_ptr<Sound> createSound(std::function<int(std::int8_t *buf, int len)> source, AudioFormat format, int samplingRate);
    };
}