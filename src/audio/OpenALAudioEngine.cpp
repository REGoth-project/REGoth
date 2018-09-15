#ifdef RE_USE_SOUND

#include "OpenALAudioEngine.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <mutex>
#include <thread>

#include <AL/al.h>
#include <AL/alc.h>

using namespace Audio;

constexpr std::size_t OPENAL_SOURCES_POOL = 256;

std::string getErrorString(ALenum error)
{
    switch (error)
    {
        case AL_NO_ERROR:
            return "No error";
        case AL_INVALID_NAME:
            return "An invalid id was passed to an OpenAL function";
        case AL_INVALID_ENUM:
            return "An invalid enum value was passed to an OpenAL function";
        case AL_INVALID_VALUE:
            return "An invalid value was passed to an OpenAL function";
        case AL_INVALID_OPERATION:
            return "The requested OpenAL operation is not valid";
        case AL_OUT_OF_MEMORY:
            return "The requested operation resulted in OpenAL running out of memory";
        default:
            return "Unknown error";
    }
}

OpenALException::OpenALException(ALenum error)
    : std::runtime_error(getErrorString(error))
    , m_err(error)
{
}

const std::vector<std::string>& OpenALAudioEngine::devices()
{
    static std::vector<std::string> enumeratedDevices;

    // The mutex is there so that static members
    // can be accessed in a thread-safe manner
    static std::mutex mtx_enumeratedDevices;

    std::lock_guard<std::mutex> lock(mtx_enumeratedDevices);

    if (enumeratedDevices.empty())
    {
        if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT"))
        {
            size_t len = 0;
            const ALCchar* devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
            const ALCchar *device = devices, *next = devices + 1;

            while (device && *device != '\0' && next && *next != '\0')
            {
                enumeratedDevices.emplace_back(device);

                len = strlen(device);
                device += (len + 1);
                next += (len + 2);
            }
        }
        else
        {
            enumeratedDevices.emplace_back();
        }
    }

    return enumeratedDevices;
}

OpenALAudioEngine::OpenALAudioEngine(const std::string& deviceName)
{
    m_device = alcOpenDevice(deviceName.empty() ? nullptr : deviceName.c_str());
    if (!m_device) throw OpenALException(alGetError());

    m_context = alcCreateContext(m_device, nullptr);
    if (!m_context) throw OpenALException(alGetError());

    captureContext();

    m_freeSources.resize(OPENAL_SOURCES_POOL);
    alGenSources(OPENAL_SOURCES_POOL, m_freeSources.data());
    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
}

OpenALAudioEngine::~OpenALAudioEngine()
{
    /* Clearing the vectors has the effect of destroying all the buffers allocated
    for the various sounds, before destroying the sources and closing the
    OpenAL device */
    m_bufferedSounds.clear();
    m_streamingSounds.clear();

    alDeleteSources(m_freeSources.size(), m_freeSources.data());
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}

void OpenALAudioEngine::captureContext() const
{
    alcMakeContextCurrent(m_context);
}

void OpenALAudioEngine::gain(float g)
{
    captureContext();
    alListenerf(AL_GAIN, g);
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) throw OpenALException(err);
}

void OpenALAudioEngine::position(const Math::float3& p)
{
    captureContext();
    alListenerfv(AL_POSITION, reinterpret_cast<const ALfloat*>(&p));
}

void OpenALAudioEngine::velocity(const Math::float3& v)
{
    captureContext();
    alListenerfv(AL_VELOCITY, reinterpret_cast<const ALfloat*>(&v));
}

void OpenALAudioEngine::orientation(const Orientation& o)
{
    captureContext();
    alListenerfv(AL_ORIENTATION, reinterpret_cast<const ALfloat*>(&o));
}

namespace Audio
{
    class OpenALSound
        : public Sound
    {
    protected:
        OpenALSound(OpenALAudioEngine* engine)
            : m_hasBuffer(false)
            , m_engine(engine)
        {}
    public:
        OpenALSound(OpenALAudioEngine* engine, const std::int16_t* buf,
                    std::size_t len, Format fmt, std::size_t samplingFreq)
            : m_hasBuffer(true)
            , m_engine(engine)
        {
            alGenBuffers(1, &m_buffer);
            ALenum error = alGetError();
            if (error != AL_NO_ERROR) throw OpenALException(error);

            alBufferData(m_buffer,
                         fmt == Format::Mono ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
                         buf, len, samplingFreq);
            error = alGetError();
            if (error != AL_NO_ERROR)
            {
                alDeleteBuffers(1, &m_buffer);
                throw OpenALException(error);
            }
        }

        virtual ~OpenALSound()
        {
            if (m_hasSource)
            {
                // Stop source and detach from buffer
                alSourceStop(m_source);
                alSourcei(m_source, AL_BUFFER, 0);
                m_engine->m_freeSources.push_back(m_source);
            }
            
            if(m_hasBuffer) alDeleteBuffers(1, &m_buffer);
        }

        ALuint seizeSource()
        {
            if (!m_hasSource)
            {
                throw std::runtime_error("Sound has no source");
            }

            m_hasSource = false;
            return m_source;
        }

        bool hasSource() const
        {
            return m_hasSource;
        }

        void pitch(float p) override
        {
            getSource();
            alSourcef(m_source, AL_PITCH, p);
            ALenum error = alGetError();
            if (error != AL_NO_ERROR) throw OpenALException(error);
        }

        void gain(float g) override
        {
            getSource();
            alSourcef(m_source, AL_GAIN, g);
            ALenum error = alGetError();
            if (error != AL_NO_ERROR) throw OpenALException(error);
        }

        void maxDistance(float md) override
        {
            getSource();
            alSourcef(m_source, AL_MAX_DISTANCE, md);
            ALenum error = alGetError();
            if (error != AL_NO_ERROR) throw OpenALException(error);
        }

        void position(const Math::float3& p) override
        {
            getSource();
            alSourcefv(m_source, AL_POSITION, reinterpret_cast<const ALfloat*>(&p));
        }

        void velocity(const Math::float3& v) override
        {
            getSource();
            alSourcefv(m_source, AL_VELOCITY, reinterpret_cast<const ALfloat*>(&v));
        }

        void direction(const Math::float3& d) override
        {
            getSource();
            alSourcefv(m_source, AL_DIRECTION, reinterpret_cast<const ALfloat*>(&d));
        }

        void relative(bool r) override
        {
            getSource();
            alSourcei(m_source, AL_SOURCE_RELATIVE, r ? AL_TRUE : AL_FALSE);
        }

        void looping(bool l) override
        {
            getSource();
            alSourcei(m_source, AL_LOOPING, l ? AL_TRUE : AL_FALSE);
        }

        State state() override
        {
            if (!m_hasSource) return State::Stopped;
            ALint s;
            alGetSourcei(m_source, AL_SOURCE_STATE, &s);
            switch (s)
            {
                case AL_PLAYING:
                    return State::Playing;
                case AL_PAUSED:
                    return State::Paused;
                default:
                    return State::Stopped;
            }
        };

        void play() override
        {
            getSource();
            alSourcePlay(m_source);
        }

        void pause() override
        {
            if (!m_hasSource) return;
            getSource();
            alSourcePause(m_source);
        }

        void stop() override
        {
            if (!m_hasSource) return;
            getSource();
            alSourceStop(m_source);
        }

    private:
        ALuint m_buffer = 0;
        bool m_hasBuffer;

    protected:
        ALuint m_source = 0;
        bool m_hasSource = false; 
        OpenALAudioEngine* m_engine;

        virtual void getSource()
        {
            if (m_hasSource) return;
            m_source = m_engine->getFreeSource();
            m_hasSource = true;
            alSourcei(m_source, AL_BUFFER, m_buffer);
        }
    };

    class StreamingOpenALSound final
        : public OpenALSound
    {
        /**
         * @brief Number of buffers to use while streaming
         * 
         */
        static constexpr std::size_t BUFFER_NUM = 3;

        /**
         * @brief Number of samples for each buffer
         * 
         * Longer buffers mean lower processing power required,
         * but higher latency and increased memory usage
         */
        static constexpr std::size_t BUFFER_SIZE = 1024;
    public:
        StreamingOpenALSound(OpenALAudioEngine* engine,
                            SoundStream stream, Format fmt, std::size_t sampleRate)
            : OpenALSound(engine)
            , m_stream(stream)
            , m_format(fmt == Format::Mono ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16)
            , m_sampleRate(sampleRate)
            , m_stop(false)
        {
            alGenBuffers(BUFFER_NUM, m_buffers.data());
            ALenum error = alGetError();
            if (error != AL_NO_ERROR) throw OpenALException(error);

            play();

            m_thread = std::thread(&StreamingOpenALSound::renderThread, this);
        }

        ~StreamingOpenALSound() override
        {
            m_stop = true;
            m_thread.join();
            if (m_hasSource)
            {
                // Stop source and detach from buffer
                alSourceStop(m_source);
                ALint queuedBuffers;
                alSourceUnqueueBuffers(m_source, BUFFER_NUM, m_buffers.data());
                alSourcei(m_source, AL_BUFFER, 0);
                m_engine->m_freeSources.push_back(m_source);
            }
            
            alDeleteBuffers(BUFFER_NUM, m_buffers.data());
        }

    private:
        SoundStream m_stream;
        ALenum m_format;
        std::size_t m_sampleRate;
        std::atomic_bool m_stop;
        std::array<ALuint, BUFFER_NUM> m_buffers{};
        std::thread m_thread;

        /**
         * @brief Renders the buffer using the streaming source
         * 
         * @param buf The buffer to render data into
         * @return true The source has more data to render
         * @return false The source has no more data to render
         */
        bool renderBuffer(std::array<std::int16_t, BUFFER_SIZE>& buf)
        {
            int rendered = m_stream(buf.data(), BUFFER_SIZE);
            if(rendered < 0) return false;

            while(rendered < BUFFER_SIZE)
            {
                if(m_stop) return false;
                int res = m_stream(buf.data() + rendered, BUFFER_SIZE - rendered);
                if(res < 0) return false;
                rendered += res;
            }
            return true;
        }

        void renderThread()
        {
            std::array<std::int16_t, BUFFER_SIZE> dataBuffer{};

            for(auto buf : m_buffers)
            {
                if(!renderBuffer(dataBuffer)) return;
                alBufferData(buf, m_format, dataBuffer.data(), BUFFER_SIZE * 2, m_sampleRate);
            }
            
            alSourceQueueBuffers(m_source, BUFFER_NUM, m_buffers.data());

            if(!renderBuffer(dataBuffer)) return;

            while(true)
            {
                if(m_stop) return;
                if(!m_hasSource) continue;

                ALint processedBuffers;
                alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processedBuffers);
                if(processedBuffers <= 0) continue;
                while(processedBuffers--)
                {
                    ALuint buffer;
                    alSourceUnqueueBuffers(m_source, 1, &buffer);
                    alBufferData(buffer, m_format, dataBuffer.data(), BUFFER_SIZE * 2, m_sampleRate);
                    alSourceQueueBuffers(m_source, 1, &buffer);
                    if(!renderBuffer(dataBuffer)) return;
                }

                if(state() != State::Playing) play();
            }
        }

        void getSource() override
        {
            if (m_hasSource) return;
            m_source = m_engine->getFreeSource();
            m_hasSource = true;
        }
    };
}  // namespace Audio

SoundPtr OpenALAudioEngine::createSound(const std::int16_t* buf, std::size_t len, Format fmt, std::size_t sampleRate)
{
    captureContext();
    auto ptr = std::make_shared<OpenALSound>(this, buf, len, fmt, sampleRate);
    m_bufferedSounds.push_back(ptr);
    return ptr;
}

SoundPtr OpenALAudioEngine::createSound(SoundStream stream, Format fmt, std::size_t sampleRate)
{
    captureContext();
    auto ptr = std::make_shared<StreamingOpenALSound>(this, stream, fmt, sampleRate);
    m_streamingSounds.push_back(ptr);
    return ptr;
}

ALuint OpenALAudioEngine::getFreeSource()
{
    captureContext();

    auto lambda = [](const auto& sound) { return sound.use_count() == 1; };

    // Remove all expired sounds
    std::remove_if(std::begin(m_bufferedSounds), std::end(m_bufferedSounds), lambda);
    std::remove_if(std::begin(m_streamingSounds), std::end(m_streamingSounds), lambda);

    if (m_freeSources.empty())
    {
        // Before generating new sources, check if an existing one can be reused
        for (const auto& snd : m_bufferedSounds)
        {
            if (snd->hasSource() && snd->state() == State::Stopped)
            {
                return snd->seizeSource();
            }
        }

        ALuint source;
        alGenSources(1, &source);
        ALenum error = alGetError();
        if(error != AL_NO_ERROR) throw OpenALException(error);
        return source;
    }
    else
    {
        ALuint source = m_freeSources.back();
        m_freeSources.pop_back();
        return source;
    }
}

#endif // RE_USE_SOUND