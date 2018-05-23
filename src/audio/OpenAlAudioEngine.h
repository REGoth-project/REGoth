#pragma once

#include "AudioEngine.h"

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

namespace Audio
{
    class OpenAlAudioEngine : public AudioEngine
    {
    public:
        OpenAlAudioEngine(const std::string& deviceName);

        ~OpenAlAudioEngine();

        void setListenerPosition(const Math::float3& pos);
        void setListenerVelocity(const Math::float3& vel);
        void setListenerOrientation(const std::array<float, 6>& orient);
        void setListenerGain(float gain);

        std::shared_ptr<Sound> createSound(const std::vector<std::int16_t>& data, AudioFormat format, int samplingRate);
        std::shared_ptr<Sound> createSound(const std::vector<std::int8_t>& data, AudioFormat format, int samplingRate);
        std::shared_ptr<Sound> createSound(std::function<int(std::int16_t *buf, int len)> source, AudioFormat format, int samplingRate);
        std::shared_ptr<Sound> createSound(std::function<int(std::int8_t *buf, int len)> source, AudioFormat format, int samplingRate);

    private:
        ALCdevice* m_Device = nullptr;
        ALCcontext* m_Context = nullptr;
    };
}