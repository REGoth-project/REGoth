#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

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

static DirectMusic::SegmentTiming getTiming(std::uint32_t v)
{
    switch (v)
    {
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
    {
        audio_engine.position({0.0f, 0.0f, 0.0f});
        audio_engine.velocity({0.0f, 0.0f, 0.0f});
        audio_engine.orientation({{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}});

        createSounds();
        initializeMusic();
    }

    void AudioWorld::initializeMusic()
    {
        std::string datPath = "/_work/data/Scripts/_compiled/MUSIC.DAT";
        std::string datFile = Utils::getCaseSensitivePath(datPath, m_Engine.getEngineArgs().gameBaseDirectory);

        if (!Utils::fileExists(datFile))
        {
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
        try
        {
            const auto sfFactory = DirectMusic::DlsPlayer::createFactory();
            m_musicContext = std::make_unique<DirectMusic::PlayingContext>(44100, 2, sfFactory);

            auto loader = [musicPath, baseDir](const std::string& name) {
                const auto search = Utils::lowered(Utils::stripFilePath(name));
                for (const auto& file : Utils::getFilesInDirectory(musicPath))
                {
                    const auto lowercaseName = Utils::lowered(Utils::stripFilePath(file));
                    if (lowercaseName == search)
                    {
                        return Utils::readBinaryFileContents(file);
                    }
                }
                return std::vector<std::uint8_t>();
            };

            m_musicContext->provideLoader(loader);

            for (const auto& segment : Utils::getFilesInDirectory(musicPath, "sgt"))
            {
                const auto lowercaseName = Utils::lowered(Utils::stripFilePath(segment));
                const auto segm = m_musicContext->loadSegment(segment);
                LogInfo() << "Loading " + segment;
                m_Segments[lowercaseName] = m_musicContext->prepareSegment(*segm);
            }
            LogInfo() << "All segments loaded.";

            m_MusicSource = m_Engine.getAudioEngine().createSound(
                [&](auto buf, auto len) -> int {
                    if (m_exiting) return -1;

                    m_musicContext->renderBlock(buf, len);
                    return len;
                },
                Audio::Format::Stereo, 44100);
            m_MusicSource->gain(1.0f);
            m_MusicSource->position({0.0f, 0.0f, 0.0f});
        }
        catch (const std::exception& exc)
        {
            LogError() << "Couldn't initialize music system: " << exc.what();
        }
    }

    AudioWorld::~AudioWorld()
    {
        m_exiting = true;

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
            if (snd->sound)  // already loaded
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

        try
        {
            auto dataPtr = reinterpret_cast<const std::int16_t*>(wav.getData());
            auto format = wav.getChannels() == 1 ? Audio::Format::Mono : Audio::Format::Stereo;
            auto sound = m_Engine.getAudioEngine().createSound(dataPtr, wav.getDataSize(), format, wav.getRate());
            snd->sound = sound;
            m_Sources.push_back(sound);
        }
        catch (const std::runtime_error& error)
        {
            static bool warned = false;
            if (!warned)
            {
                LogWarn() << "Could not create sound: " << error.what();
                warned = true;
            }
            return Handle::SfxHandle::makeInvalidHandle();
        }

        // Load other versions, such as randomly played footstep variants
        loadVariants(h);

        m_SoundMap[name] = h;

        return h;
    }

    Handle::SfxHandle AudioWorld::loadAudioVDF(const std::string& name)
    {
        return loadAudioVDF(m_VDFSIndex, name);
    }

    Audio::SoundPtr AudioWorld::playSound(Handle::SfxHandle h, const Math::float3& position, bool relative, float maxDist)
    {
        Sound& snd = m_Allocator.getElement(h);

        try
        {
            snd.sound->pitch(m_Engine.getGameClock().getGameEngineSpeedFactor());
            snd.sound->gain(snd.sfx.vol / 127.0f);
            snd.sound->position(position);
            snd.sound->velocity({0.0f, 0.0f, 0.0f});
            snd.sound->maxDistance(maxDist);
            snd.sound->relative(relative);
            snd.sound->looping(snd.sfx.loop);
            // TODO: proper looping would require slicing and queueing multiple buffers
            // and setting the source to loop when the non-looping buffer was played.
            // start and end don't seem to be used, thoug?
            snd.sound->play();
        }
        catch (const std::runtime_error& err)
        {
            static bool warned = false;
            if (!warned)
            {
                LogWarn() << "Error while playing sound: " << err.what();
                warned = true;
            }
            return nullptr;
        }

        return snd.sound;
    }

    Handle::SfxHandle AudioWorld::allocateSound(const std::string& name, const Daedalus::GEngineClasses::C_SFX& sfx)
    {
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
        for (auto& s : m_Sources) s->stop();
    }

    void AudioWorld::pauseSounds()
    {
        for (auto& s : m_Sources) s->pause();
    }

    void AudioWorld::continueSounds()
    {
        for (auto& s : m_Sources)
        {
            if (s->state() == Audio::State::Paused)
            {
                s->play();
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

    Audio::SoundPtr AudioWorld::playSound(Handle::SfxHandle h)
    {
        return playSound(h, Math::float3(0, 0, 0), true);
    }

    Audio::SoundPtr AudioWorld::playSound(const std::string& name)
    {
        // Check if that sound has already been loaded. If not, load it now.
        Handle::SfxHandle h = loadAudioVDF(name);

        // Check if loading was successfull, if so, play it
        if (!h.isValid())
        {
            return nullptr;
        }

        return playSound(h, Math::float3(0, 0, 0), true);
    }

    Audio::SoundPtr AudioWorld::playSound(Handle::SfxHandle h, const Math::float3& position, float maxDist)
    {
        return playSound(h, position, false, maxDist);
    }

    Audio::SoundPtr AudioWorld::playSound(const std::string& name, const Math::float3& position, float maxDist)
    {
        // Check if that sound has already been loaded. If not, load it now.
        Handle::SfxHandle h = loadAudioVDF(name);

        // Check if loading was successfull, if so, play it
        if (!h.isValid())
        {
            return nullptr;
        }

        return playSound(h, position, false, maxDist);
    }

    Audio::SoundPtr AudioWorld::playSoundVariantRandom(const std::string& name, const Math::float3& position, float maxDist)
    {
        // Check if that sound has already been loaded. If not, load it now.
        Handle::SfxHandle h = loadAudioVDF(name);

        // Check if loading was successfull, if so, play it
        if (!h.isValid())
        {
            return nullptr;
        }

        return playSoundVariantRandom(h, position, maxDist);
    }

    Audio::SoundPtr AudioWorld::playSoundVariantRandom(Handle::SfxHandle h, const Math::float3& position, float maxDist)
    {
        Sound& snd = m_Allocator.getElement(h);

        return playSound(snd.variants[rand() % snd.variants.size()], position, false, maxDist);
    }

    Audio::SoundPtr AudioWorld::playSoundVariantRandom(const std::string& name)
    {
        // Check if that sound has already been loaded. If not, load it now.
        Handle::SfxHandle h = loadAudioVDF(name);

        // Check if loading was successfull, if so, play it
        if (!h.isValid())
        {
            return nullptr;
        }

        return playSoundVariantRandom(h);
    }

    Audio::SoundPtr AudioWorld::playSoundVariantRandom(Handle::SfxHandle h)
    {
        Sound& snd = m_Allocator.getElement(h);

        return playSound(snd.variants[rand() % snd.variants.size()]);
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

    std::vector<std::string> AudioWorld::getLoadedSegments() const
    {
        std::vector<std::string> vect;
        for (const auto& kvp : m_Segments)
        {
            vect.push_back(kvp.first);
        }
        return vect;
    }
}  // namespace World
