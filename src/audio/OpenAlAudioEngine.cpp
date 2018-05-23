#include <cstring>
#include <string>
#include <thread>
#include <AL/al.h>
#include <AL/alc.h>
#include <utils/logger.h>

#include "OpenAlAudioEngine.h"

#define RE_NUM_AUDIO_BUFFERS 3
#define RE_AUDIO_BUFFERS_LEN 1024

using namespace Audio;

static void enumerateDevices(std::vector<std::string>& enumerated)
{
    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"))
    {
        size_t len = 0;
        const ALCchar* devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
        const ALCchar *device = devices, *next = devices + 1;

        while (device && *device != '\0' && next && *next != '\0')
        {
            enumerated.push_back(device);

            len = strlen(device);
            device += (len + 1);
            next += (len + 2);
        }
    }

    if (enumerated.empty())
        enumerated.push_back(std::string());  // empty string is default device
}

static std::string getErrorString(size_t errorCode)
{
    switch (errorCode)
    {
        case AL_NO_ERROR:
            return "AL_NO_ERROR";
        case AL_INVALID_NAME:
            return "AL_INVALID_NAME";
        case AL_INVALID_ENUM:
            return "AL_INVALID_ENUM";
        case AL_INVALID_VALUE:
            return "AL_INVALID_VALUE";
        case AL_INVALID_OPERATION:
            return "AL_INVALID_OPERATION";
        case AL_OUT_OF_MEMORY:
            return "AL_OUT_OF_MEMORY";
    }
    return "UNKNOWN";
}

OpenAlAudioEngine::OpenAlAudioEngine(const std::string& deviceName)
{
    m_Device = alcOpenDevice(deviceName.empty() ? NULL : deviceName.c_str());
    if (!m_Device)
    {
        LogWarn() << "Could not open audio device '" << (deviceName.empty() ? "default" : deviceName) << "': "
                  << getErrorString(alGetError()) << ", sound disabled";
        return;
    }

    m_Context = alcCreateContext(m_Device, nullptr);
    if (!m_Context)
    {
        LogWarn() << "Could not create OpenAL context: "
                  << getErrorString(alcGetError(m_Device));
        return;
    }

    alcMakeContextCurrent(m_Context);

    // Need this for AL_MAX_DISTANCE to work
    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
}

OpenAlAudioEngine::~OpenAlAudioEngine()
{
    if (m_Context)
    {
        alcDestroyContext(m_Context);
    }

    if (m_Device)
    {
        alcCloseDevice(m_Device);
    }
}

void OpenAlAudioEngine::setListenerPosition(const Math::float3& pos)
{
    alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
}

void OpenAlAudioEngine::setListenerVelocity(const Math::float3& vel)
{
    alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
}

void OpenAlAudioEngine::setListenerOrientation(const std::array<float, 6>& orient)
{
    alListenerfv(AL_ORIENTATION, orient.data());
}

void OpenAlAudioEngine::setListenerGain(float gain)
{
    alListenerf(AL_GAIN, gain);
}

class BaseOpenAlSound : public Sound
{
    friend class OpenAlAudioEngine;

public:
    bool isPlaying() const
    {
        ALint state;
        alGetSourcei(m_source, AL_SOURCE_STATE, &state);

        return state == AL_PLAYING;
    }

    bool isLooping() const
    {
        int looping;
        alGetSourcei(m_source, AL_LOOPING, &looping);
        return looping == AL_TRUE;
    }

    bool isPaused() const
    {
        ALint state;
        alGetSourcei(m_source, AL_SOURCE_STATE, &state);

        return state == AL_PAUSED;
    }

    bool isActive() const override
    {
        ALint state;
        alGetSourcei(m_source, AL_SOURCE_STATE, &state);

        return state == AL_PLAYING || state == AL_PAUSED;
    }

    void stop()
    {
        alSourceStop(m_source);
    }

    void pause()
    {
        alSourcePause(m_source);
    }

    void play()
    {
        alSourcePlay(m_source);
    }

    const Math::float3& getPosition() const
    {
        Math::float3 vec;
        alGetSourcefv(m_source, AL_POSITION, vec.v);
        return vec;
    }

    const Math::float3& getVelocity() const
    {
        Math::float3 vec;
        alGetSourcefv(m_source, AL_VELOCITY, vec.v);
        return vec;
    }

    float getPitch() const
    {
        float pitch;
        alGetSourcef(m_source, AL_PITCH, &pitch);
        return pitch;
    }

    float getMaxDistance() const
    {
        float maxDist;
        alGetSourcef(m_source, AL_MAX_DISTANCE, &maxDist);
        return maxDist;
    }

    float getGain() const
    {
        float gain;
        alGetSourcef(m_source, AL_GAIN, &gain);
        return gain;
    }

    bool isRelative() const
    {
        int isRelative;
        alGetSourcei(m_source, AL_SOURCE_RELATIVE, &isRelative);
        return isRelative == AL_TRUE;
    }

    void setPosition(const Math::float3& pos)
    {
        alSourcefv(m_source, AL_POSITION, pos.v);
    }

    void setVelocity(const Math::float3& vel)
    {
        alSourcefv(m_source, AL_VELOCITY, vel.v);
    }

    void setPitch(float pitch)
    {
        alSourcef(m_source, AL_PITCH, pitch);
    }

    void setMaxDistance(float dist)
    {
        alSourcef(m_source, AL_PITCH, dist);
    }

    void setGain(float gain)
    {
        alSourcef(m_source, AL_GAIN, gain);
    }

    void setRelative(bool isRelative)
    {
        alSourcei(m_source, AL_SOURCE_RELATIVE, isRelative ? AL_TRUE : AL_FALSE);
    }

    void setLooping(bool looping)
    {
        alSourcei(m_source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
    }

    ~BaseOpenAlSound()
    {
        alDeleteSources(1, &m_source);
    }

protected:
    ALuint m_source;
    BaseOpenAlSound(ALuint source)
        : m_source(source)
    {
    }
};

class SimpleOpenAlSound : public BaseOpenAlSound
{
    friend class OpenAlAudioEngine;

public:
    ~SimpleOpenAlSound()
    {
        alDeleteBuffers(1, &m_buffer);
    }

protected:
    ALuint m_buffer;

    SimpleOpenAlSound(ALuint source, ALuint buffer)
        : BaseOpenAlSound(source)
        , m_buffer(buffer)
    {
        alSourcei(m_source, AL_BUFFER, buffer);
    }
};

std::shared_ptr<Sound> OpenAlAudioEngine::createSound(const std::vector<std::int16_t>& data, AudioFormat format, int samplingRate)
{
    alcMakeContextCurrent(m_Context);
    ALuint buffer, source;
    alGenBuffers(1, &buffer);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        LogWarn() << "Could not create OpenAL buffer: "
                  << getErrorString(error);
        return nullptr;
    }

    alBufferData(buffer,
                 format == AudioFormat::Stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
                 data.data(), data.size() * 2, samplingRate);
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        LogWarn() << "Could not set OpenAL buffer data: "
                  << getErrorString(error);
        return nullptr;
    }

    alGenSources(1, &source);
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        LogWarn() << "Could not create OpenAL source: "
                  << getErrorString(error);
        return nullptr;
    }

    return std::shared_ptr<SimpleOpenAlSound>(new SimpleOpenAlSound(source, buffer));
}

std::shared_ptr<Sound> OpenAlAudioEngine::createSound(const std::vector<std::int8_t>& data, AudioFormat format, int samplingRate)
{
    alcMakeContextCurrent(m_Context);
    ALuint buffer, source;
    alGenBuffers(1, &buffer);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        LogWarn() << "Could not create OpenAL buffer: "
                  << getErrorString(error);
        return nullptr;
    }

    alBufferData(buffer,
                 format == AudioFormat::Stereo ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8,
                 data.data(), data.size(), samplingRate);
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        LogWarn() << "Could not set OpenAL buffer data: "
                  << getErrorString(error);
        return nullptr;
    }

    alGenSources(1, &source);
    error = alGetError();
    if (error != AL_NO_ERROR)
    {
        LogWarn() << "Could not create OpenAL source: "
                  << getErrorString(error);
        return nullptr;
    }

    return std::shared_ptr<SimpleOpenAlSound>(new SimpleOpenAlSound(source, buffer));
}

template <typename T, size_t numBuffers, size_t bufLength>
class StreamingOpenAlSound : public BaseOpenAlSound
{
    friend class OpenAlAudioEngine;

public:
    ~StreamingOpenAlSound()
    {
        m_isClosing = true;
        m_bufferThread.join();
        alDeleteBuffers(numBuffers, m_buffers.data());
    }

protected:
    std::thread m_bufferThread;
    std::array<ALuint, numBuffers> m_buffers;
    std::function<int(T* buf, int len)> m_func;
    ALenum m_format;
    int m_samplingRate;
    bool m_isClosing = false;
    StreamingOpenAlSound(ALuint source, std::function<int(T* buf, int len)> func, ALenum format, int samplingRate)
        : BaseOpenAlSound(source)
        , m_func(func)
        , m_samplingRate(samplingRate)
        , m_format(format)
    {
        alGenBuffers(numBuffers, m_buffers.data());
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            LogWarn() << "Could not create OpenAL buffers: "
                      << getErrorString(error);
            return;
        }

        m_bufferThread = std::thread([&]() {
            std::array<T, bufLength> buf;
            buf.fill(0);
            for (size_t i = 0; i < numBuffers; i++)
            {
                alBufferData(m_buffers[i], m_format, buf.data(), bufLength * sizeof(T), m_samplingRate);
            }

            alSourceQueueBuffers(m_source, numBuffers, m_buffers.data());

            while (!m_isClosing)
            {
                ALint val;
                alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &val);
                if (val <= 0)
                {
                    continue;
                }

                for (int i = 0; i < val; i++)
                {
                    ALuint buffer;
                    alSourceUnqueueBuffers(m_source, 1, &buffer);
                    alBufferData(buffer, m_format, buf.data(), bufLength * sizeof(T), m_samplingRate);
                    alSourceQueueBuffers(m_source, 1, &buffer);
                    ALenum error = alGetError();
                    if (error != AL_NO_ERROR)
                    {
                        LogError() << "Error while buffering: " << getErrorString(error);
                        return;
                    }

                    int pos = 0;
                    while (pos < bufLength)
                    {
                        int res = m_func(buf.data() + pos, bufLength - pos);
                        if (res == 0)
                        {
                            return;
                        }
                        pos += res;
                    }
                }
            }
        });
    }
};

std::shared_ptr<Sound> OpenAlAudioEngine::createSound(std::function<int(std::int16_t* buf, int len)> func, AudioFormat format, int samplingRate)
{
    alcMakeContextCurrent(m_Context);
    ALuint source;
    alGenSources(1, &source);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        LogWarn() << "Could not create OpenAL source: "
                  << getErrorString(error);
        return nullptr;
    }

    return std::shared_ptr<StreamingOpenAlSound<std::int16_t, RE_NUM_AUDIO_BUFFERS, RE_AUDIO_BUFFERS_LEN>>(new StreamingOpenAlSound<std::int16_t, RE_NUM_AUDIO_BUFFERS, RE_AUDIO_BUFFERS_LEN>(source, func,
                                                                                                                                                                                              format == AudioFormat::Mono ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, samplingRate));
}

std::shared_ptr<Sound> OpenAlAudioEngine::createSound(std::function<int(std::int8_t* buf, int len)> func, AudioFormat format, int samplingRate)
{
    alcMakeContextCurrent(m_Context);
    ALuint source;
    alGenSources(1, &source);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        LogWarn() << "Could not create OpenAL source: "
                  << getErrorString(error);
        return nullptr;
    }

    return std::shared_ptr<StreamingOpenAlSound<std::int8_t, RE_NUM_AUDIO_BUFFERS, RE_AUDIO_BUFFERS_LEN>>(new StreamingOpenAlSound<std::int8_t, RE_NUM_AUDIO_BUFFERS, RE_AUDIO_BUFFERS_LEN>(source, func,
                                                                                                                                                                                            format == AudioFormat::Mono ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8, samplingRate));
}