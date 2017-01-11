#include <cstddef>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include <adpcm/adpcm-lib.h>
#include <utils/logger.h>

#include "AudioEngine.h"
#include "AudioWorld.h"
#include "WavReader.h"

using namespace Audio;

namespace World
{

    AudioWorld::AudioWorld(AudioEngine &engine, const VDFS::FileIndex &vdfidx) :
        m_VDFSIndex(vdfidx)
    {
        m_Context = alcCreateContext(engine.getDevice(), nullptr);
        if (!m_Context)
        {
            LogWarn() << "Could not create OpenAL context: "
                      << AudioEngine::getErrorString(alcGetError(engine.getDevice()));
            return;
        }

        alcMakeContextCurrent(m_Context);

        alListener3f(AL_POSITION, 0, 0, 0.0f);
        // check for errors
        alListener3f(AL_VELOCITY, 0, 0, 0);
        // check for errors
        ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
        alListenerfv(AL_ORIENTATION, listenerOri);
    }

    AudioWorld::~AudioWorld()
    {
        for (int i=0; i<Config::MAX_NUM_LEVEL_AUDIO_FILES; i++)
        {
            AudioFile &snd = m_Allocator.getElements()[i];
            if (snd.m_Handle != 0)
                alDeleteBuffers(1, &snd.m_Handle);
        }

        for (Source &src : m_Sources)
            alDeleteSources(1, &src.m_Handle);

        if (m_Context)
            alcDestroyContext(m_Context);
    }

    Handle::AudioHandle AudioWorld::loadAudioVDF(const VDFS::FileIndex& idx, const std::string& name)
    {
    #ifdef RE_USE_SOUND
        if (!m_Context)
            return Handle::AudioHandle::makeInvalidHandle();

        // Check cache first
        if(m_SoundMap.find(name) != m_SoundMap.end())
            return m_SoundMap[name];

        LogInfo() << "Loading sound: " << name;

        // Load the audio-file from the VDF-archive
        std::vector<uint8_t> data;
        idx.getFileData(name, data);

        if(data.empty())
            return Handle::AudioHandle::makeInvalidHandle();

        {
            FILE *fp = fopen(name.c_str(), "wb");
            fwrite(&data[0], 1, data.size(), fp);
            fclose(fp);
        }

        WavReader wav(&data[0], data.size());
        if (!wav.open() || !wav.read())
            return Handle::AudioHandle::makeInvalidHandle();

        alcMakeContextCurrent(m_Context);

        Handle::AudioHandle h = m_Allocator.createObject();
        AudioFile& a = m_Allocator.getElement(h);
        alGenBuffers(1, &a.m_Handle);

        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            static bool warned = false;
            if (!warned)
            {
                LogWarn() << "Could not create OpenAL buffer: "
                          << AudioEngine::getErrorString(error);
                return Handle::AudioHandle::makeInvalidHandle();
            }
        }

        alBufferData(a.m_Handle, AL_FORMAT_MONO16, wav.getData(), wav.getDataSize(), wav.getRate());
        if (error != AL_NO_ERROR)
        {
            static bool warned = false;
            if (!warned)
            {
                LogWarn() << "Could not set OpenAL buffer data: "
                          << AudioEngine::getErrorString(error);
            }
            return Handle::AudioHandle::makeInvalidHandle();
        }

        m_SoundMap[name] = h;

        return h;
    #else
        return Handle::AudioHandle::makeInvalidHandle();
    #endif
    }

    Handle::AudioHandle AudioWorld::loadAudioVDF(const std::string& name)
    {
        return loadAudioVDF(m_VDFSIndex, name);
    }

    void AudioWorld::playSound(Handle::AudioHandle h)
    {
    #ifdef RE_USE_SOUND

        if (!m_Context)
            return;

        alcMakeContextCurrent(m_Context);

        AudioFile& a = m_Allocator.getElement(h);

        // Get a cached source object
        Source s = getFreeSource();

        alSourcei(s.m_Handle, AL_BUFFER, a.m_Handle);
//        alSourceQueueBuffers(s.m_Handle, 1, &a.m_Handle);
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            static bool warned = false;
            if (!warned)
            {
                LogWarn() << "Could not attach buffer to source: " << AudioEngine::getErrorString(error);
                warned = true;
            }
            return;
        }

        alSourcePlay(s.m_Handle);
        error = alGetError();
        if (error != AL_NO_ERROR)
        {
            static bool warned = false;
            if (!warned)
            {
                LogWarn() << "Could not start source!" << AudioEngine::getErrorString(error);
                warned = true;
            }
            return;
        }
    #endif
    }

    void AudioWorld::playSound(const std::string& name)
    {
        auto it = m_SoundMap.find(name);
        if(it == m_SoundMap.end())
        {
            // Did not find that, try to load it...
            Handle::AudioHandle h = loadAudioVDF(name);

            // Check if loading was successfull, if so, play it
            if(!h.isValid())
                return;

            playSound(h);
        }else
        {
            // No need to load it again, just play it
            playSound((*it).second);
        }
    }

    #ifdef RE_USE_SOUND
    AudioWorld::Source AudioWorld::getFreeSource()
    {
        if (!m_Context)
            return { 0 };

        alcMakeContextCurrent(m_Context);

        // Check if we could re-use one
        for (const Source& s : m_Sources)
        {
            ALint state;
            alGetSourcei(s.m_Handle, AL_SOURCE_STATE, &state);

            if (state != AL_PLAYING && state != AL_PAUSED)
                return s;
        }

        ALuint source;
        alGenSources(1, &source);

        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            static bool warned = false;
            if (!warned)
            {
                LogWarn() << "Could not allocate AL source!";
                warned = true;
            }
            return { 0 };
        }

        alSourcef(source, AL_PITCH, 1);
        // check for errors
        alSourcef(source, AL_GAIN, 1);
        // check for errors
        alSource3f(source, AL_POSITION, 0, 0, 0);
        // check for errors
        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        // check for errors
        alSourcei(source, AL_LOOPING, AL_FALSE);

        // Nothing to re-use available, make a new entry
        m_Sources.push_back({ source });
        return m_Sources.back();
    }
    #endif

    void AudioWorld::stopSounds()
    {
    #ifdef RE_USE_SOUND
        if (!m_Context)
            return;

        alcMakeContextCurrent(m_Context);

        for (Source& s : m_Sources)
            alSourceStop(s.m_Handle);
#endif
    }

}
