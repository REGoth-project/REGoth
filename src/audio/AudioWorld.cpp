#include <cstddef>
#include <functional>
#include <vector>
#include <cstdint>

#ifdef RE_USE_SOUND
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include <dmusic/PlayingContext.h>
#ifndef DMUSIC_TSF_SUPPORT
#define DMUSIC_TSF_SUPPORT 1
#endif
#include <dmusic/SoundFontPlayer.h>
#endif

#include <adpcm/adpcm-lib.h>

#include <stdlib.h>
#include <daedalus/DATFile.h>
#include <daedalus/DaedalusGameState.h>
#include <daedalus/DaedalusVM.h>
#include <logic/ScriptEngine.h>
#include <utils/logger.h>

#include "engine/BaseEngine.h"

#include "AudioEngine.h"
#include "AudioWorld.h"
#include "WavReader.h"

using namespace Audio;

namespace World
{
    AudioWorld::AudioWorld(Engine::BaseEngine& engine, AudioEngine& audio_engine, const VDFS::FileIndex& vdfidx)
        : m_Engine(engine)
        , m_VDFSIndex(vdfidx)
        , m_exiting(false)
    {
#ifdef RE_USE_SOUND
        if (!audio_engine.getDevice())
            return;

        m_Context = alcCreateContext(audio_engine.getDevice(), nullptr);
        if (!m_Context)
        {
            LogWarn() << "Could not create OpenAL context: "
                      << AudioEngine::getErrorString(alcGetError(audio_engine.getDevice()));
            return;
        }

        alcMakeContextCurrent(m_Context);

        alListener3f(AL_POSITION, 0, 0, 0.0f);
        // check for errors
        alListener3f(AL_VELOCITY, 0, 0, 0);
        // check for errors
        ALfloat listenerOri[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
        alListenerfv(AL_ORIENTATION, listenerOri);

        // Need this for AL_MAX_DISTANCE to work
        alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

        createSounds();

        initializeMusic();
#endif
    }

#ifdef RE_USE_SOUND
    void AudioWorld::initializeMusic()
    {
        // DirectMusic initialization
        std::string baseDir = m_Engine.getEngineArgs().gameBaseDirectory;
        std::string musicPath = Utils::getCaseSensitivePath("/_work/data/Music", baseDir);
        const auto sfFactory = DirectMusic::SoundFontPlayer::createFactory("Orchestra.sf2");
        m_MusicContext = std::make_unique<DirectMusic::PlayingContext>(44100, 2, sfFactory);

        auto loader = [musicPath, baseDir](const std::string& name)
        {
            const auto search = Utils::lowered(Utils::stripFilePath(name));
            for (const auto& file : Utils::getFilesInDirectory(musicPath))
            {
                const auto lowercaseName = Utils::lowered(Utils::stripFilePath(file));
                if (lowercaseName == search)
                {
                    return Utils::readBinaryFileContents(Utils::getCaseSensitivePath(file));
                }
            }
            return std::vector<std::uint8_t>();
        };

        m_MusicContext->provideLoader(loader);

        for (const auto& segment : Utils::getFilesInDirectory(musicPath, "sgt"))
        {
            const auto lowercaseName = Utils::lowered(Utils::stripFilePath(segment));
            const auto segm = m_MusicContext->loadSegment(segment);
            LogInfo() << "Loading " + segment;
            m_Segments[lowercaseName] = m_MusicContext->prepareSegment(*segm);
        }
        LogInfo() << "All segments loaded.";

        alGenBuffers(RE_NUM_MUSIC_BUFFERS, m_musicBuffers);
        alGenSources(1, &m_musicSource);

        // Set the default volume
        alSourcef(m_musicSource, AL_GAIN, 1);

        // Set the default position of the sound
        alSource3f(m_musicSource, AL_POSITION, 0, 0, 0);

        m_musicRenderThread = std::thread(&AudioWorld::musicRenderFunction, this);
    }

    void AudioWorld::musicRenderFunction()
    {
        ALenum error;
        std::int16_t* buf = new std::int16_t[RE_MUSIC_BUFFER_LEN];
        for (int i = 0; i < RE_NUM_MUSIC_BUFFERS; i++)
        {
            m_MusicContext->renderBlock(buf, RE_MUSIC_BUFFER_LEN);
            alBufferData(m_musicBuffers[i], AL_FORMAT_STEREO16, buf, RE_MUSIC_BUFFER_LEN, 44100);
        }

        alSourceQueueBuffers(m_musicSource, RE_NUM_MUSIC_BUFFERS, m_musicBuffers);
        alSourcePlay(m_musicSource);
        error = alGetError();
        if (error != AL_NO_ERROR)
        {
            LogError() << "Cannot start playing music: " << AudioEngine::getErrorString(error);
            return;
        }

        while (!m_exiting)
        {
            ALint val;
            int n = 0;
            alGetSourcei(m_musicSource, AL_BUFFERS_PROCESSED, &val);
            error = alGetError();
            if (val <= 0)
            {
                continue;
            }

            for(int i = 0; i < val; i++)
            {
                ALuint buffer;
                m_MusicContext->renderBlock(buf, RE_MUSIC_BUFFER_LEN);
                alSourceUnqueueBuffers(m_musicSource, 1, &buffer);
                alBufferData(buffer, AL_FORMAT_STEREO16, buf, RE_MUSIC_BUFFER_LEN, 44100);
                alSourceQueueBuffers(m_musicSource, 1, &buffer);
                error = alGetError();
                if (error != AL_NO_ERROR)
                {
                    LogError() << "Error while buffering: " << AudioEngine::getErrorString(error);
                    return;
                }
            }

            alGetSourcei(m_musicSource, AL_SOURCE_STATE, &val);
            if (val != AL_PLAYING)
                alSourcePlay(m_musicSource);
        }
    }
#endif

    AudioWorld::~AudioWorld()
    {
#ifdef RE_USE_SOUND
        m_exiting = true;
        m_musicRenderThread.join();

        alDeleteBuffers(RE_NUM_MUSIC_BUFFERS, m_musicBuffers);
        alDeleteSources(1, &m_musicSource);

        for (int i = 0; i < Config::MAX_NUM_LEVEL_AUDIO_FILES; i++)
        {
            Sound& snd = m_Allocator.getElements()[i];
            if (snd.m_Handle != 0)
                alDeleteBuffers(1, &snd.m_Handle);
        }

        for (Source& src : m_Sources)
            alDeleteSources(1, &src.m_Handle);

        if (m_Context)
            alcDestroyContext(m_Context);

        delete m_VM;
#endif
    }

    Handle::SfxHandle AudioWorld::loadAudioVDF(const VDFS::FileIndex& idx, const std::string& name)
    {
#ifdef RE_USE_SOUND
        if (!m_Context)
            return Handle::SfxHandle::makeInvalidHandle();

        std::string ucname = name;
        std::transform(ucname.begin(), ucname.end(), ucname.begin(), ::toupper);

        // m_SoundMap contains all the sounds with C_SFX script definitions
        Sound* snd = nullptr;
        Handle::SfxHandle h = m_SoundMap[ucname];
        if (!h.isValid())
        {
            // there are sounds which have no C_SFX defined
            Daedalus::GEngineClasses::C_SFX sfx;
            sfx.file = idx.hasFile(ucname) ? ucname : (ucname + ".wav");
            h = allocateSound(ucname, sfx);
            snd = &m_Allocator.getElement(h);
            m_SoundMap[ucname] = h;
        }
        else
        {
            snd = &m_Allocator.getElement(h);
            if (snd->m_Handle)  // already loaded
                return h;
        }

        // Load the audio-file from the VDF-archive
        std::vector<uint8_t> data;
        idx.getFileData(snd->sfx.file, data);

        if (data.empty())
            return Handle::SfxHandle::makeInvalidHandle();

        WavReader wav(&data[0], data.size());
        if (!wav.open() || !wav.read())
            return Handle::SfxHandle::makeInvalidHandle();

        alGenBuffers(1, &snd->m_Handle);

        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            static bool warned = false;
            if (!warned)
            {
                LogWarn() << "Could not create OpenAL buffer: "
                          << AudioEngine::getErrorString(error);
                return Handle::SfxHandle::makeInvalidHandle();
            }
            return Handle::SfxHandle::makeInvalidHandle();
        }

        alBufferData(snd->m_Handle, AL_FORMAT_MONO16, wav.getData(), wav.getDataSize(), wav.getRate());
        error = alGetError();
        if (error != AL_NO_ERROR)
        {
            static bool warned = false;
            if (!warned)
            {
                LogWarn() << "Could not set OpenAL buffer data: "
                          << AudioEngine::getErrorString(error);
            }
            return Handle::SfxHandle::makeInvalidHandle();
        }

        // Load other versions, such as randomly played footstep variants
        loadVariants(h);

        m_SoundMap[name] = h;

        return h;
#else
        return Handle::SfxHandle::makeInvalidHandle();
#endif
    }

    Handle::SfxHandle AudioWorld::loadAudioVDF(const std::string& name)
    {
        return loadAudioVDF(m_VDFSIndex, name);
    }

    Utils::Ticket<AudioWorld> AudioWorld::playSound(Handle::SfxHandle h, const Math::float3& position, bool relative, float maxDist)
    {
#ifdef RE_USE_SOUND

        if (!m_Context)
            return Utils::Ticket<AudioWorld>();

        alcMakeContextCurrent(m_Context);

        Sound& snd = m_Allocator.getElement(h);

        // Get a cached source object
        Source s = getFreeSource();

        //LogInfo() << "play sound " << snd.sfx.file << " vol " << snd.sfx.vol;

        alSourcef(s.m_Handle, AL_PITCH, m_Engine.getGameClock().getGameEngineSpeedFactor());
        alSourcef(s.m_Handle, AL_GAIN, snd.sfx.vol / 127.0f);
        alSource3f(s.m_Handle, AL_POSITION, position.x, position.y, position.z);
        alSource3f(s.m_Handle, AL_VELOCITY, 0, 0, 0);
        alSourcef(s.m_Handle, AL_MAX_DISTANCE, maxDist);

        // Relative for sources directly attached to the listener
        alSourcei(s.m_Handle, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);

        // TODO: proper looping would require slicing and queueing multiple buffers
        // and setting the source to loop when the non-looping buffer was played.
        // start and end don't seem to be used, thoug?
        alSourcei(s.m_Handle, AL_LOOPING, snd.sfx.loop ? AL_TRUE : AL_FALSE);

        alSourcei(s.m_Handle, AL_BUFFER, snd.m_Handle);
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            static bool warned = false;
            if (!warned)
            {
                LogWarn() << "Could not attach buffer to source: " << AudioEngine::getErrorString(error);
                warned = true;
            }
            return Utils::Ticket<AudioWorld>();
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
            return Utils::Ticket<AudioWorld>();
        }
        return s.soundTicket;
#else
        return Utils::Ticket<AudioWorld>();
#endif
    }

#ifdef RE_USE_SOUND
    AudioWorld::Source AudioWorld::getFreeSource()
    {
        if (!m_Context)
            return AudioWorld::Source();

        alcMakeContextCurrent(m_Context);

        // Check if we could re-use one
        for (Source& s : m_Sources)
        {
            ALint state;
            alGetSourcei(s.m_Handle, AL_SOURCE_STATE, &state);

            if (state != AL_PLAYING && state != AL_PAUSED)
            {
                // reusing old source, give new ticket to it
                s.soundTicket = Utils::Ticket<AudioWorld>();
                return s;
            }
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
            return AudioWorld::Source();
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
        m_Sources.emplace_back();
        m_Sources.back().m_Handle = source;
        return m_Sources.back();
    }

    Handle::SfxHandle AudioWorld::allocateSound(const std::string& name, const Daedalus::GEngineClasses::C_SFX& sfx)
    {
        /*LogInfo() << "alloc sound " << name << " file " << sfx.file << " vol: " << sfx.vol
                  << " loop: " << sfx.loop << " loop start: " << sfx.loopStartOffset << " loop end: " << sfx.loopEndOffset;
         */
        Handle::SfxHandle h = m_Allocator.createObject();
        Sound& snd = m_Allocator.getElement(h);
        snd.sfx = sfx;
        snd.m_Handle = 0;
        snd.name = name;

        m_SoundMap[name] = h;
        return h;
    }

    void AudioWorld::createSound(const std::string& name, const Daedalus::GEngineClasses::C_SFX& sfx)
    {
        allocateSound(name, sfx);
    }

    void AudioWorld::createSounds()
    {
        std::string datPath = "/_work/data/Scripts/_compiled/SFX.DAT";
        std::string datFile = Utils::getCaseSensitivePath(datPath, m_Engine.getEngineArgs().gameBaseDirectory);

        if (!Utils::fileExists(datFile))
        {
            LogError() << "Failed to find SFX.DAT at: " << datFile;
            return;
        }

        m_VM = new Daedalus::DaedalusVM(datFile);
        Daedalus::registerGothicEngineClasses(*m_VM);

        size_t count = 0;
        m_VM->getDATFile().iterateSymbolsOfClass("C_SFX", [&](size_t i, Daedalus::PARSymbol& s) {

            Daedalus::GameState::SfxHandle h = m_VM->getGameState().createSfx();
            Daedalus::GEngineClasses::C_SFX& sfx = m_VM->getGameState().getSfx(h);
            m_VM->initializeInstance(ZMemory::toBigHandle(h), i, Daedalus::IC_Sfx);

            createSound(s.name, sfx);

            count++;
        });

        LogInfo() << "created " << count << " sounds";
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

    void AudioWorld::stopSound(Utils::Ticket<AudioWorld> ticket)
    {
#ifdef RE_USE_SOUND
        if (!m_Context)
            return;

        alcMakeContextCurrent(m_Context);

        for (Source& s : m_Sources)
        {
            if (s.soundTicket == ticket)
            {
                alSourceStop(s.m_Handle);
                return;
            }
        }
#endif
    }

    bool AudioWorld::soundIsPlaying(Utils::Ticket<AudioWorld> ticket)
    {
#ifdef RE_USE_SOUND
        if (!m_Context)
            return false;

        alcMakeContextCurrent(m_Context);

        for (Source& s : m_Sources)
        {
            if (s.soundTicket == ticket)
            {
                ALint state;
                alGetSourcei(s.m_Handle, AL_SOURCE_STATE, &state);
                return state == AL_PLAYING || state == AL_PAUSED;
            }
        }
        return false;
#else
        return false;
#endif
    }

    void AudioWorld::pauseSounds()
    {
#ifdef RE_USE_SOUND
        if (!m_Context)
            return;

        alcMakeContextCurrent(m_Context);

        for (Source& s : m_Sources)
        {
            ALint state;
            alGetSourcei(s.m_Handle, AL_SOURCE_STATE, &state);
            if (state == AL_PLAYING)
            {
                alSourcePause(s.m_Handle);
            }
        }
#endif
    }

    void AudioWorld::continueSounds()
    {
#ifdef RE_USE_SOUND
        if (!m_Context)
            return;

        alcMakeContextCurrent(m_Context);

        for (Source& s : m_Sources)
        {
            ALint state;
            alGetSourcei(s.m_Handle, AL_SOURCE_STATE, &state);
            if (state == AL_PAUSED)
            {
                alSourcePlay(s.m_Handle);
            }
        }
#endif
    }

    void AudioWorld::loadVariants(Handle::SfxHandle sfx)
    {
        Sound& snd = m_Allocator.getElement(sfx);
        snd.variants.push_back(sfx);  // Add self to variants

        Handle::SfxHandle v;
        int i = 1;

        // Variants start at "_A1". Load sounds until one fails.
        do
        {
            v = loadAudioVDF(snd.name + "_A" + std::to_string(i));

            if (v.isValid())
            {
                snd.variants.push_back(v);
            }

            i++;
        } while (v.isValid());
    }

    void AudioWorld::setListenerPosition(const Math::float3& position)
    {
        alListener3f(AL_POSITION, position.x, position.y, position.z);
    }

    void AudioWorld::setListenerVelocity(const Math::float3& velocity)
    {
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }

    void AudioWorld::setListenerOrientation(const Math::float3& at, const Math::float3& up)
    {
        float v[6] = {at.x, at.y, at.z, up.x, up.y, up.z};
        alListenerfv(AL_ORIENTATION, v);
    }

    Utils::Ticket<AudioWorld> AudioWorld::playSound(Handle::SfxHandle h)
    {
        return playSound(h, Math::float3(0, 0, 0), true);
    }

    Utils::Ticket<AudioWorld> AudioWorld::playSound(const std::string& name)
    {
        // Check if that sound has already been loaded. If not, load it now.
        Handle::SfxHandle h = loadAudioVDF(name);

        // Check if loading was successfull, if so, play it
        if (!h.isValid())
        {
            return Utils::Ticket<AudioWorld>();
        }

        return playSound(h, Math::float3(0, 0, 0), true);
    }

    Utils::Ticket<AudioWorld> AudioWorld::playSound(Handle::SfxHandle h, const Math::float3& position, float maxDist)
    {
        return playSound(h, position, false, maxDist);
    }

    Utils::Ticket<AudioWorld> AudioWorld::playSound(const std::string& name, const Math::float3& position, float maxDist)
    {
        // Check if that sound has already been loaded. If not, load it now.
        Handle::SfxHandle h = loadAudioVDF(name);

        // Check if loading was successfull, if so, play it
        if (!h.isValid())
        {
            return Utils::Ticket<AudioWorld>();
        }

        return playSound(h, position, false, maxDist);
    }

    Utils::Ticket<AudioWorld> AudioWorld::playSoundVariantRandom(const std::string& name, const Math::float3& position, float maxDist)
    {
        // Check if that sound has already been loaded. If not, load it now.
        Handle::SfxHandle h = loadAudioVDF(name);

        // Check if loading was successfull, if so, play it
        if (!h.isValid())
        {
            return Utils::Ticket<AudioWorld>();
        }

        return playSoundVariantRandom(h, position, maxDist);
    }

    Utils::Ticket<AudioWorld> AudioWorld::playSoundVariantRandom(Handle::SfxHandle h, const Math::float3& position, float maxDist)
    {
        Sound& snd = m_Allocator.getElement(h);

        return playSound(snd.variants[rand() % snd.variants.size()], position, false, maxDist);
    }

    Utils::Ticket<AudioWorld> AudioWorld::playSoundVariantRandom(const std::string& name)
    {
        // Check if that sound has already been loaded. If not, load it now.
        Handle::SfxHandle h = loadAudioVDF(name);

        // Check if loading was successfull, if so, play it
        if (!h.isValid())
        {
            return Utils::Ticket<AudioWorld>();
        }

        return playSoundVariantRandom(h);
    }

    Utils::Ticket<AudioWorld> AudioWorld::playSoundVariantRandom(Handle::SfxHandle h)
    {
        Sound& snd = m_Allocator.getElement(h);

        return playSound(snd.variants[rand() % snd.variants.size()]);
    }

    void AudioWorld::setListenerGain(float gain)
    {
        alListenerf(AL_GAIN, gain);
    }

    void AudioWorld::setSoundMaxDistance(Utils::Ticket<AudioWorld> sound, float maxDist)
    {
        for (Source& s : m_Sources)
        {
            if (s.soundTicket == sound)
            {
                alSourcef(s.m_Handle, AL_MAX_DISTANCE, maxDist);
                return;
            }
        }
    }

    bool AudioWorld::playSegment(const std::string& name)
    {
#ifdef RE_USE_SOUND
        std::string loweredName = Utils::lowered(name);
        if (m_Segments.find(loweredName) == m_Segments.end())
        {
            return false;
        }
        else
        {
            m_MusicContext->playSegment(m_Segments.at(loweredName));
            return true;
        }
#endif RE_USE_SOUND
        return false;
    }

    const std::vector<std::string> AudioWorld::getLoadedSegments() const
    {
        std::vector<std::string> vect;
#ifdef RE_USE_SOUND
        for (const auto& kvp : m_Segments)
        {
            vect.push_back(kvp.first);
        }
#endif
        return vect;
    }
}
