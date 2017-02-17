#include <cstddef>
#include <functional>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include <adpcm/adpcm-lib.h>

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

    AudioWorld::AudioWorld(Engine::BaseEngine &engine, AudioEngine &audio_engine, const VDFS::FileIndex &vdfidx) :
        m_Engine(engine),
        m_VDFSIndex(vdfidx)
    {
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
        ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
        alListenerfv(AL_ORIENTATION, listenerOri);

        createSounds();
    }

    AudioWorld::~AudioWorld()
    {
        for (int i=0; i<Config::MAX_NUM_LEVEL_AUDIO_FILES; i++)
        {
            Sound &snd = m_Allocator.getElements()[i];
            if (snd.m_Handle != 0)
                alDeleteBuffers(1, &snd.m_Handle);
        }

        for (Source &src : m_Sources)
            alDeleteSources(1, &src.m_Handle);

        if (m_Context)
            alcDestroyContext(m_Context);

        delete m_VM;
    }

    Handle::SfxHandle AudioWorld::loadAudioVDF(const VDFS::FileIndex& idx, const std::string& name)
    {
    #ifdef RE_USE_SOUND
        if (!m_Context)
            return Handle::SfxHandle::makeInvalidHandle();

        std::string ucname = name;
        std::transform(ucname.begin(), ucname.end(), ucname.begin(), ::toupper);

        // m_SoundMap contains all the sounds with C_SFX script definitions
        Sound *snd = nullptr;
        Handle::SfxHandle h = m_SoundMap[ucname];
        if (!h.isValid())
        {
            // there are sounds which have no C_SFX defined
            Daedalus::GEngineClasses::C_SFX sfx;
            sfx.file = idx.hasFile(ucname) ? ucname : (ucname + ".wav");
            h = allocateSound(ucname, sfx);
            snd = &m_Allocator.getElement(h);
            m_SoundMap[ucname] = h;
        } else
        {
            snd = &m_Allocator.getElement(h);
            if (snd->m_Handle) // already loaded
                return h;
        }

        // Load the audio-file from the VDF-archive
        std::vector<uint8_t> data;
        idx.getFileData(snd->sfx.file, data);

        if(data.empty())
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

        m_SoundMap[name] = h;

        return h;
    #else
        return Handle::AudioHandle::makeInvalidHandle();
    #endif
    }

    Handle::SfxHandle AudioWorld::loadAudioVDF(const std::string& name)
    {
        return loadAudioVDF(m_VDFSIndex, name);
    }

    Utils::Ticket<AudioWorld> AudioWorld::playSound(Handle::SfxHandle h)
    {
    #ifdef RE_USE_SOUND

        if (!m_Context)
            return Utils::Ticket<AudioWorld>();

        alcMakeContextCurrent(m_Context);

        Sound& snd = m_Allocator.getElement(h);

        // Get a cached source object
        Source s = getFreeSource();

        LogInfo() << "play sound " << snd.sfx.file << " vol " << snd.sfx.vol;

        // TODO: pitch?
        //alSourcef(source, AL_PITCH, snd.sfx.);
        alSourcef(s.m_Handle, AL_GAIN, snd.sfx.vol / 127.0f);
        alSource3f(s.m_Handle, AL_POSITION, 0, 0, 0);
        alSource3f(s.m_Handle, AL_VELOCITY, 0, 0, 0);
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
        return Ticket<AudioWorld>();
    #endif
    }

    Utils::Ticket<AudioWorld> AudioWorld::playSound(const std::string& name)
    {
        auto it = m_SoundMap.find(name);
        if(it == m_SoundMap.end())
        {
            // Did not find that, try to load it...
            Handle::SfxHandle h = loadAudioVDF(name);

            // Check if loading was successfull, if so, play it
            if(!h.isValid())
            {
                return Utils::Ticket<AudioWorld>();
            }

            return playSound(h);
        }else
        {
            // No need to load it again, just play it
            return playSound((*it).second);
        }
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

    Handle::SfxHandle AudioWorld::allocateSound(const std::string &name, const Daedalus::GEngineClasses::C_SFX &sfx)
    {
        /*LogInfo() << "alloc sound " << name << " file " << sfx.file << " vol: " << sfx.vol
                  << " loop: " << sfx.loop << " loop start: " << sfx.loopStartOffset << " loop end: " << sfx.loopEndOffset;
*/
        Handle::SfxHandle h = m_Allocator.createObject();
        Sound& snd = m_Allocator.getElement(h);
        snd.sfx = sfx;
        snd.m_Handle = 0;

        m_SoundMap[name] = h;
        return h;
    }

    void AudioWorld::createSound(const std::string &name, const Daedalus::GEngineClasses::C_SFX &sfx)
    {
        allocateSound(name, sfx);
    }

    void AudioWorld::createSounds()
    {
        std::string datPath = "/_work/data/Scripts/_compiled/SFX.DAT";
        std::string datFile = Utils::getCaseSensitivePath(datPath, m_Engine.getEngineArgs().gameBaseDirectory);

        if(!Utils::fileExists(datFile))
        {
            LogError() << "Failed to find SFX.DAT at: " << datFile;
            return;
        }

        m_VM = new Daedalus::DaedalusVM(datFile);
        Daedalus::registerDaedalusStdLib(*m_VM);
        Daedalus::registerGothicEngineClasses(*m_VM);

        size_t count = 0;
        m_VM->getDATFile().iterateSymbolsOfClass("C_SFX", [&](size_t i, Daedalus::PARSymbol& s){

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

    bool AudioWorld::soundHasStopped(Utils::Ticket<AudioWorld> ticket)
    {
    #ifdef RE_USE_SOUND
        if (!m_Context)
            return true;

        alcMakeContextCurrent(m_Context);

        for (Source& s : m_Sources)
        {
            if (s.soundTicket == ticket)
            {
                ALint state;
                alGetSourcei(s.m_Handle, AL_SOURCE_STATE, &state);
                return state != AL_PLAYING && state != AL_PAUSED;
            }
        }
        return true;
    #else
        return true;
    #endif
    }
}
