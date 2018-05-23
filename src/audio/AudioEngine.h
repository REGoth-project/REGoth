#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "math/mathlib.h"

namespace Audio
{
    enum class AudioFormat
    {
        Mono,
        Stereo
    };

    class Sound
    {
    public:
        virtual bool isPlaying() const = 0;
        virtual bool isLooping() const = 0;
        virtual bool isPaused() const = 0;
        virtual bool isActive() const
        {
            return isPlaying() || isPaused();
        }

        virtual void stop() = 0;
        virtual void pause() = 0;
        virtual void play() = 0;

        const virtual Math::float3& getPosition() const = 0;
        const virtual Math::float3& getVelocity() const = 0;
        virtual float getPitch() const = 0;
        virtual float getMaxDistance() const = 0;
        virtual float getGain() const = 0;
        virtual bool isRelative() const = 0;

        virtual void setPosition(const Math::float3& pos) = 0;
        virtual void setVelocity(const Math::float3& vel) = 0;
        virtual void setPitch(float pitch) = 0;
        virtual void setMaxDistance(float dist) = 0;
        virtual void setGain(float gain) = 0;
        virtual void setRelative(bool isRelative) = 0;
        virtual void setLooping(bool looping) = 0;

        ~Sound() {}
    };

    /** The AudioEngine represents the target OS sound system.
     *
     */
    class AudioEngine
    {
    public:
        virtual void setListenerPosition(const Math::float3& pos) = 0;
        virtual void setListenerVelocity(const Math::float3& vel) = 0;
        virtual void setListenerOrientation(const std::array<float, 6>& orient) = 0;
        void setListenerOrientation(const Math::float3& at, const Math::float3& up)
        {
            setListenerOrientation({at.x, at.y, at.z, up.x, up.y, up.z});
        }
        virtual void setListenerGain(float gain) = 0;

        /// Creates a single-buffer sound with 16bit PCM sample data
        virtual std::shared_ptr<Sound> createSound(const std::vector<std::int16_t>& data, AudioFormat format, int samplingRate) = 0;

        /// Creates a single-buffer sound with 8bit PCM sample data
        virtual std::shared_ptr<Sound> createSound(const std::vector<std::int8_t>& data, AudioFormat format, int samplingRate) = 0;

        /// Creates a streaming sound with 16bit PCM sample data
        virtual std::shared_ptr<Sound> createSound(std::function<int(std::int16_t* buf, int len)> source, AudioFormat format, int samplingRate) = 0;

        /// Creates a streaming sound with 8bit PCM sample data
        virtual std::shared_ptr<Sound> createSound(std::function<int(std::int8_t* buf, int len)> source, AudioFormat format, int samplingRate) = 0;
    };
}
