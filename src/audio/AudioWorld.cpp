#include <cstddef>
#include <functional>
#include <vector>
#include <cstdint>

#include <dmusic/PlayingContext.h>
#ifndef DMUSIC_DLS_PLAYER
#define DMUSIC_DLS_PLAYER 1
#endif
#include <dmusic/DlsPlayer.h>

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

static DirectMusic::SegmentTiming getTiming(std::uint32_t v) {
    switch (v) {
    case Daedalus::GEngineClasses::TRANSITION_SUB_TYPE_BEAT:
        return DirectMusic::SegmentTiming::Beat;
    case Daedalus::GEngineClasses::TRANSITION_SUB_TYPE_MEASURE:
        return DirectMusic::SegmentTiming::Measure;

    default:
        return DirectMusic::SegmentTiming::Immediate;
    }
}

namespace World
{
    AudioWorld::AudioWorld(Engine::BaseEngine& engine, AudioEngine& audio_engine, const VDFS::FileIndex& vdfidx)
        : m_Engine(engine)
        , m_VDFSIndex(vdfidx)
        , m_exiting(false)
        , m_AudioEngine(audio_engine)
    {
        m_AudioEngine.setListenerPosition(Math::float3(0, 0, 0));
        m_AudioEngine.setListenerVelocity(Math::float3(0, 0, 0));
        m_AudioEngine.setListenerOrientation({0, 0, 1, 0, 1, 0});

        createSounds();

        initializeMusic();
    }

    void AudioWorld::initializeMusic()
    {
        std::string datPath = "/_work/data/Scripts/_compiled/MUSIC.DAT";
        std::string datFile = Utils::getCaseSensitivePath(datPath, m_Engine.getEngineArgs().gameBaseDirectory);

        if (!Utils::fileExists(datFile)) {
            LogError() << "Failed to find MUSIC.DAT at: " << datFile;
            return;
        }

        m_MusicVM = new Daedalus::DaedalusVM(datFile);
        Daedalus::registerGothicEngineClasses(*m_MusicVM);

        m_MusicVM->getDATFile().iterateSymbolsOfClass("C_MUSICTHEME", [&](size_t i, Daedalus::PARSymbol& s) {
            Daedalus::GameState::MusicThemeHandle h = m_MusicVM->getGameState().createMusicTheme();
            Daedalus::GEngineClasses::C_MusicTheme& mt = m_MusicVM->getGameState().getMusicTheme(h);
            m_MusicVM->initializeInstance(ZMemory::toBigHandle(h), i, Daedalus::IC_MusicTheme);

            m_musicThemeSegments[s.name] = mt.file;
        });

        // DirectMusic initialization
        std::string baseDir = m_Engine.getEngineArgs().gameBaseDirectory;
        std::string musicPath = Utils::getCaseSensitivePath("/_work/data/Music", baseDir);
        try {
            const auto sfFactory = DirectMusic::DlsPlayer::createFactory();
            m_musicContext = std::make_unique<DirectMusic::PlayingContext>(44100, 2, sfFactory);

            auto loader = [musicPath, baseDir](const std::string& name) {
                const auto search = Utils::lowered(Utils::stripFilePath(name));
                for (const auto& file : Utils::getFilesInDirectory(musicPath)) {
                    const auto lowercaseName = Utils::lowered(Utils::stripFilePath(file));
                    if (lowercaseName == search) {
                        return Utils::readBinaryFileContents(file);
                    }
                }
                return std::vector<std::uint8_t>();
            };

            m_musicContext->provideLoader(loader);

            for (const auto& segment : Utils::getFilesInDirectory(musicPath, "sgt")) {
                const auto lowercaseName = Utils::lowered(Utils::stripFilePath(segment));
                const auto segm = m_musicContext->loadSegment(segment);
                LogInfo() << "Loading " + segment;
                m_Segments[lowercaseName] = m_musicContext->prepareSegment(*segm);
            }
            LogInfo() << "All segments loaded.";

            m_musicSound = m_AudioEngine.createSound([&](int16_t* buf, int len) -> int {
                m_musicContext->renderBlock(buf, len);
                return len;
            }, Audio::AudioFormat::Stereo, 44100);
            m_musicSound->setGain(1);
            m_musicSound->setPosition(Math::float3(0, 0, 0));
            m_musicSound->play();
        } catch (const std::exception& exc) {
            LogError() << "Couldn't initialize music system: " << exc.what();
        }
    }

    AudioWorld::~AudioWorld()
    {
        m_exiting = true;

        m_musicSound->stop();
        m_musicSound = nullptr;

        for (int i = 0; i < Config::MAX_NUM_LEVEL_AUDIO_FILES; i++)
        {
            Sound& snd = m_Allocator.getElements()[i];
            snd.sound = nullptr;
        }

        for (Source& src : m_Sources)
            src.m_Sound = nullptr;

        delete m_SoundVM;
        delete m_MusicVM;
    }

    Handle::SfxHandle AudioWorld::loadAudioVDF(const VDFS::FileIndex& idx, const std::string& name)
    {
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
            if (snd->sound != nullptr)  // already loaded
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

        std::vector<std::int16_t> rawData(wav.getDataVector().begin(), wav.getDataVector().end());

        snd->sound = m_AudioEngine.createSound(rawData, Audio::AudioFormat::Mono, wav.getRate());

        // Load other versions, such as randomly played footstep variants
        loadVariants(h);

        m_SoundMap[name] = h;

        return h;
    }

    Handle::SfxHandle AudioWorld::loadAudioVDF(const std::string& name)
    {
        return loadAudioVDF(m_VDFSIndex, name);
    }

    Utils::Ticket<AudioWorld> AudioWorld::playSound(Handle::SfxHandle h, const Math::float3& position, bool relative, float maxDist)
    {
        Sound& snd = m_Allocator.getElement(h);

        // Get a cached source object
        Source s = getFreeSource();

        //LogInfo() << "play sound " << snd.sfx.file << " vol " << snd.sfx.vol;

        snd.sound->setPitch(m_Engine.getGameClock().getGameEngineSpeedFactor());
        snd.sound->setGain(snd.sfx.vol / 127.0f);
        snd.sound->setPosition(position);
        snd.sound->setVelocity(Math::float3(0, 0, 0));
        snd.sound->setMaxDistance(maxDist);

        // Relative for sources directly attached to the listener
        snd.sound->setRelative(relative);

        // TODO: proper looping would require slicing and queueing multiple buffers
        // and setting the source to loop when the non-looping buffer was played.
        // start and end don't seem to be used, thoug?
        snd.sound->setLooping(snd.sfx.loop);

        snd.sound->play();
        s.m_Sound = snd.sound;
        return s.soundTicket;
    }

    AudioWorld::Source AudioWorld::getFreeSource()
    {
        // Check if we could re-use one
        for (Source& s : m_Sources)
        {
            if (s.m_Sound == nullptr || (!s.m_Sound->isPlaying() && !s.m_Sound->isPaused()))
            {
                // reusing old source, give new ticket to it
                s.soundTicket = Utils::Ticket<AudioWorld>();
                return s;
            }
        }

        // Nothing to re-use available, make a new entry
        m_Sources.emplace_back();
        m_Sources.back().m_Sound = nullptr;
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
        snd.sound = nullptr;
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

        m_SoundVM = new Daedalus::DaedalusVM(datFile);
        Daedalus::registerGothicEngineClasses(*m_SoundVM);

        size_t count = 0;
        m_SoundVM->getDATFile().iterateSymbolsOfClass("C_SFX", [&](size_t i, Daedalus::PARSymbol& s) {

            Daedalus::GameState::SfxHandle h = m_SoundVM->getGameState().createSfx();
            Daedalus::GEngineClasses::C_SFX& sfx = m_SoundVM->getGameState().getSfx(h);
            m_SoundVM->initializeInstance(ZMemory::toBigHandle(h), i, Daedalus::IC_Sfx);

            createSound(s.name, sfx);

            count++;
        });

        LogInfo() << "created " << count << " sounds";
    }


    void AudioWorld::stopSounds()
    {
        for (Source& s : m_Sources)
            s.m_Sound->stop();
    }

    void AudioWorld::stopSound(Utils::Ticket<AudioWorld> ticket)
    {
        for (Source& s : m_Sources)
        {
            if (s.soundTicket == ticket && s.m_Sound != nullptr)
            {
                s.m_Sound->stop();
                return;
            }
        }
    }

    bool AudioWorld::soundIsPlaying(Utils::Ticket<AudioWorld> ticket)
    {
        for (Source& s : m_Sources)
        {
            if (s.soundTicket == ticket)
            {
                return s.m_Sound != nullptr && (s.m_Sound->isPlaying() || s.m_Sound->isPaused());
            }
        }
        return false;
    }

    void AudioWorld::pauseSounds()
    {
        for (Source& s : m_Sources)
        {
            if (s.m_Sound != nullptr && s.m_Sound->isPlaying())
            {
                s.m_Sound->pause();
            }
        }
    }

    void AudioWorld::continueSounds()
    {
        for (Source& s : m_Sources)
        {
            if (s.m_Sound != nullptr && s.m_Sound->isPaused())
            {
                s.m_Sound->play();
            }
        }
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
        m_AudioEngine.setListenerPosition(position);
    }

    void AudioWorld::setListenerVelocity(const Math::float3& velocity)
    {
        m_AudioEngine.setListenerVelocity(velocity);
    }

    void AudioWorld::setListenerOrientation(const Math::float3& at, const Math::float3& up)
    {
        m_AudioEngine.setListenerOrientation(at, up);
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
        m_AudioEngine.setListenerGain(gain);
    }

    void AudioWorld::setSoundMaxDistance(Utils::Ticket<AudioWorld> sound, float maxDist)
    {
        for (Source& s : m_Sources)
        {
            if (s.soundTicket == sound)
            {
                s.m_Sound->setMaxDistance(maxDist);
                return;
            }
        }
    }

    bool AudioWorld::playSegment(const std::string& name, DirectMusic::SegmentTiming timing)
    {
        std::string loweredName = Utils::lowered(name);
        if (m_musicContext == nullptr || m_Segments.find(loweredName) == m_Segments.end())
        {
            return false;
        }
        else
        {
            if (m_playingSegment != loweredName)
            {
                m_musicContext->playSegment(m_Segments.at(loweredName), timing);
                m_playingSegment = loweredName;
            }
            return true;
        }
        return false;
    }

    bool AudioWorld::playMusicTheme(const std::string& name)
    {
        if (m_MusicVM->getDATFile().hasSymbolName(Utils::uppered(name)))
        {
            size_t i = m_MusicVM->getDATFile().getSymbolIndexByName(Utils::uppered(name));
            Daedalus::GameState::MusicThemeHandle h = m_MusicVM->getGameState().createMusicTheme();
            Daedalus::GEngineClasses::C_MusicTheme& mt = m_MusicVM->getGameState().getMusicTheme(h);
            m_MusicVM->initializeInstance(ZMemory::toBigHandle(h), i, Daedalus::IC_MusicTheme);

            return playSegment(mt.file, getTiming(mt.transSubType));
        }
        return false;
    }

    const std::vector<std::string> AudioWorld::getLoadedSegments() const
    {
        std::vector<std::string> vect;
        for (const auto& kvp : m_Segments)
        {
            vect.push_back(kvp.first);
        }
        return vect;
    }
}
