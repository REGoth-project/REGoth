#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include <math/mathlib.h>

namespace Audio
{
    enum class State {
        Stopped,
        Paused,
        Playing
    };

    enum class Format {
        Mono,
        Stereo
    };

    /**
     * @brief Sounds represent audio sources.
     * 
     * All of the information about a sound should be re-set before it is played
     * after it had been stopped.
     * 
     */
    class Sound {
    public:
        /**
         * @brief Set sound pitch, must be positive
         * 
         * 1 means original pitch, values < 1 mean lower, > 1 mean higher.
         * 
         * @return float 
         */
        virtual void pitch(float) = 0;

        /**
         * @brief Set sound gain
         * 
         * 1 means original volume, <1 means lower gain, >1 means higher.
         * 
         * @return float 
         */
        virtual void gain(float) = 0;

        /**
         * @brief Set the maximum distance from which this sound can be heard.
         *        Must be positive.
         * 
         */
        virtual void maxDistance(float) = 0;
        
        /**
         * @brief Set the coordinates of the sound's position.
         * 
         */
        virtual void position(const Math::float3&) = 0;

        /**
         * @brief Set the components of the sound's velocity.
         * 
         */
        virtual void velocity(const Math::float3&) = 0;

        /**
         * @brief Set the componenets of the sound's direction.
         * 
         */
        virtual void direction(const Math::float3&) = 0;

        /**
         * @brief Set whether the sound's position is relative to the listener.
         * 
         */
        virtual void relative(bool) = 0;

        /**
         * @brief Set wheter the sound should loop back to the beginning once it
         *        finishes playing.
         * 
         * This is ignored in the case of streming sounds
         */
        virtual void looping(bool) = 0;

        /**
         * @brief Get the playback state of this sound.
         * 
         * @return State 
         */
        virtual State state() = 0;
        
        /**
         * @brief Starts sound playback.
         * 
         * This method is idempotent: if the sound was already playing,
         * nothing happens.
         */
        virtual void play() = 0;
        /**
         * @brief Pauses sound playback.
         * 
         * This method is idempotent: if the sound was already paused,
         * nothing happens.
         */
        virtual void pause() = 0;
        /**
         * @brief Stops sound playback.
         * 
         * Playback will resume from the beginning.
         * This method is idempotent: if the sound was already stopped,
         * nothing happens.
         */
        virtual void stop() = 0;
    };

    using SoundPtr = std::shared_ptr<Sound>;
    using SoundStream = std::function<int(std::int16_t* buf, std::size_t size)>;

    struct Orientation {
        Math::float3 at;
        Math::float3 up;
    };

    /** An AudioEngine is the interface to the system's audio driver
     *
     */
    class AudioEngine
    {
    public:
        /**
         * @brief Set master listener gain, must be positive
         * 
         * 1 means original volume, <1 means lower gain, >1 means higher.
         * 
         */
        virtual void gain(float) = 0;

        /**
         * @brief Set coordinates of listener's position
         * 
         */
        virtual void position(const Math::float3&) = 0;

        /**
         * @brief Set components of listener's velocity
         * 
         */
        virtual void velocity(const Math::float3&) = 0;

        /**
         * @brief Set listener's orientation
         * 
         */
        virtual void orientation(const Orientation&) = 0;

        /**
         * @brief Creates a sound object from an audio buffer
         * 
         * @return SoundPtr 
         */
        virtual SoundPtr createSound(const std::int16_t* buf, std::size_t len, Format, std::size_t samplingFreq) = 0;
        
        /**
         * @brief Creates a sound object from an audio stream
         * 
         * @return SoundPtr 
         */
        virtual SoundPtr createSound(SoundStream, Format, std::size_t samplingFreq) = 0;
    };
}
