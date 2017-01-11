#include <cstddef>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include <adpcm/adpcm-lib.h>
#include <utils/logger.h>

#include "AudioEngine.h"
#include "AudioWorld.h"

using namespace Engine;

namespace World
{

    AudioWorld::AudioWorld(Engine::AudioEngine &engine, const VDFS::FileIndex &vdfidx) :
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

    int AudioWorld::adpcm_decode_data(uint8_t* infile, std::vector<uint8_t>& outfile, size_t num_samples, int num_channels , int block_size )
    {
        int samples_per_block = (block_size - num_channels * 4) * (num_channels ^ 3) + 1;
        int16_t *pcm_block;
        uint8_t *adpcm_block;

        while (num_samples) {
            int this_block_adpcm_samples = samples_per_block;
            int this_block_pcm_samples = samples_per_block;

            if (this_block_adpcm_samples > num_samples) {
                this_block_adpcm_samples = ((num_samples + 6) & ~7) + 1;
                block_size = (this_block_adpcm_samples - 1) / (num_channels ^ 3) + (num_channels * 4);
                this_block_pcm_samples = num_samples;
            }

            // Read in-wav
            adpcm_block = infile;
            infile += block_size;

            // Add to out-wav
            size_t ofidx = outfile.size();
            outfile.resize(outfile.size() + this_block_pcm_samples * num_channels * 2);
            pcm_block = (int16_t*)&outfile[ofidx];

            if (adpcm_decode_block(pcm_block, adpcm_block, block_size, num_channels) != this_block_adpcm_samples) {
                fprintf(stderr, "adpcm_decode_block() did not return expected value!\n");
                return -1;
            }

            num_samples -= this_block_pcm_samples;
        }

        return 0;
    }

    static bool loadWav(std::vector<uint8_t> &dest, uint32_t &sample_rate, const std::vector<uint8_t> &source)
    {
        uint16_t num_channels = *reinterpret_cast<const uint16_t*>(&source[22]);
        sample_rate = *reinterpret_cast<const uint32_t*>(&source[24]);
        uint16_t block_size = *reinterpret_cast<const uint16_t*>(&source[32]);
        uint32_t samples_per_block = (block_size - num_channels * 4) * (num_channels ^ 3) + 1;

        uint32_t num_blocks = (source.size() - 60) / block_size;
        int32_t num_samples = (source.size() - 60) * 2;

        const uint8_t *sourcep = &source[60];

        while (num_samples > 0) {
            int this_block_adpcm_samples = samples_per_block;
            int this_block_pcm_samples = samples_per_block;

            if (this_block_adpcm_samples > num_samples) {
                this_block_adpcm_samples = ((num_samples + 6) & ~7) + 1;
                block_size = (this_block_adpcm_samples - 1) / (num_channels ^ 3) + (num_channels * 4);
                this_block_pcm_samples = num_samples;
            }

            printf("offset: %d\n", (int)(sourcep - &source[0]));
            printf("block adpcm size: %d\n", this_block_adpcm_samples);
            printf("block pcm size: %d\n", this_block_pcm_samples);

            // Add to out-wav
            size_t ofidx = dest.size();
            dest.resize(dest.size() + this_block_pcm_samples * num_channels * 2);

            adpcm_decode_block(reinterpret_cast<int16_t*>(&dest[ofidx]), sourcep, block_size, num_channels);
            /*
            if (adpcm_decode_block(reinterpret_cast<int16_t*>(&dest[ofidx]), sourcep, block_size, num_channels) != this_block_adpcm_samples) {
                LogWarn() << "adpcm_decode_block() failed";
                return false;
            }
            */

            sourcep += block_size;
            num_samples -= this_block_pcm_samples;
        }

        return true;
    }

    Handle::AudioHandle AudioWorld::loadAudioVDF(const VDFS::FileIndex& idx, const std::string& name)
    {
    #ifdef RE_USE_SOUND
        if (!m_Context)
            return Handle::AudioHandle::makeInvalidHandle();

        std::vector<uint8_t> data;
        std::vector<uint8_t> outData;
        // Check cache first
        if(m_SoundMap.find(name) != m_SoundMap.end())
            return m_SoundMap[name];

        LogInfo() << "Loading sound: " << name;

        // Load the audio-file from the VDF-archive
        idx.getFileData(name, data);

        if(data.empty())
            return Handle::AudioHandle::makeInvalidHandle();

        {
            FILE *fp = fopen(name.c_str(), "wb");
            fwrite(&data[0], 1, data.size(), fp);
            fclose(fp);
        }

        uint32_t sample_rate;
        std::vector<uint8_t> raw;
        if (!loadWav(raw, sample_rate, data))
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

//        alBufferData(a.m_Handle, AL_FORMAT_MONO16, outData.data(), outData.size(), 22050);
        alBufferData(a.m_Handle, AL_FORMAT_MONO16, raw.data(), raw.size(), sample_rate);
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

#if 0
        // Create the buffer-object
        Handle::AudioHandle h = m_Allocator.createObject();

        // Decode the ADPCM compressed audio gothic uses

        // Gothics wav-files have a headersize of 60 bytes, 1 channel, blocksize of 1024
        size_t numNibbles = (data.size() - 60) * 2;
        adpcm_decode_data(data.data()+60, outData, numNibbles);

        {
            FILE *fp = fopen((name + ".raw").c_str(), "wb");
            fwrite(&outData[0], 1, outData.size(), fp);
            fclose(fp);
        }

        alcMakeContextCurrent(m_Context);

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

//        alBufferData(a.m_Handle, AL_FORMAT_MONO16, outData.data(), outData.size(), 22050);
        alBufferData(a.m_Handle, AL_FORMAT_MONO_MSADPCM_SOFT, data.data() + 60, data.size() - 60, 22050);
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
#endif

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
