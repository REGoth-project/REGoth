#pragma once

#include "AudioEngine.h"

#include <string>

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;
typedef int ALenum;
typedef unsigned int ALuint;

namespace Audio
{
    class OpenALSound;
    class StreamingOpenALSound;
    class OpenALAudioEngine final
        : public AudioEngine
    {
        friend OpenALSound;
        friend StreamingOpenALSound;
    public:
        static const std::vector<std::string>& devices();

        OpenALAudioEngine(const std::string& device = "");
        ~OpenALAudioEngine();

        void gain(float) override;

        void position(const Math::float3&) override;

        void velocity(const Math::float3&) override;

        void orientation(const Orientation&) override;

        SoundRef createSound(const std::int16_t* buf, std::size_t len, Format, std::size_t) override;
        
        SoundRef createSound(SoundStream, Format, std::size_t) override;
    private:
        ALCdevice* m_device = nullptr;
        ALCcontext* m_context = nullptr;
        std::vector<ALuint> m_freeSources;

        // Keep reference of created sounds to ensure
        // they are deleted before the engine is deallocated
        std::vector<std::shared_ptr<OpenALSound>> m_bufferedSounds;
        std::vector<std::shared_ptr<OpenALSound>> m_streamingSounds;

        ALuint getFreeSource();

        void captureContext() const;
    };

    class OpenALException
        : public std::runtime_error
    {
    public:
        OpenALException(ALenum error);

        ALenum error() const { return m_err; }
    private:
        ALenum m_err;
    };
}