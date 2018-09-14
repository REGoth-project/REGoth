#pragma once

#include <list>
#include <map>
#include <thread>

#include <glm/glm.hpp>

#include <dmusic/PlayingContext.h>
#include <daedalus/DaedalusStdlib.h>
#include <handle/HandleDef.h>
#include <memory/Config.h>
#include <utils/Utils.h>
#include <vdfs/fileIndex.h>
#include <audio/AudioEngine.h>

typedef struct ALCcontext_struct ALCcontext;

namespace Audio
{
    class AudioEngine;
}

namespace Engine
{
    class BaseEngine;
}

namespace Daedalus
{
    class DaedalusVM;

    namespace GEngineClasses
    {
        struct C_SFX;
    }
}

namespace Logic
{
    class ScriptEngine;
}

namespace World
{
    /** The audio world class.
     *
     * Each world has it's own AudioWorld which manages an OpenAL context.
     *
     */
    class AudioWorld
    {
        friend class Audio::AudioEngine;

    public:
        AudioWorld(Engine::BaseEngine& engine, Audio::AudioEngine& audio_engine, const VDFS::FileIndex& vdfidx);

        virtual ~AudioWorld();

        /**
         * @brief Loads an audio-file from the given or stored VDFS-FileIndex
         */
        Handle::SfxHandle loadAudioVDF(const VDFS::FileIndex& idx, const std::string& name);

        Handle::SfxHandle loadAudioVDF(const std::string& name);

        /**
         * Plays the sound of the given handle/name
         */
        Audio::SoundPtr playSound(Handle::SfxHandle h, const Math::float3& position, bool relative, float maxDist = FLT_MAX);
        Audio::SoundPtr playSound(Handle::SfxHandle h);
        Audio::SoundPtr playSound(const std::string& name);
        Audio::SoundPtr playSoundVariantRandom(const std::string& name);
        Audio::SoundPtr playSoundVariantRandom(Handle::SfxHandle h);

        Audio::SoundPtr playSound(Handle::SfxHandle h, const Math::float3& position, float maxDist = FLT_MAX);
        Audio::SoundPtr playSound(const std::string& name, const Math::float3& position, float maxDist = FLT_MAX);
        Audio::SoundPtr playSoundVariantRandom(const std::string& name, const Math::float3& position, float maxDist = FLT_MAX);
        Audio::SoundPtr playSoundVariantRandom(Handle::SfxHandle h, const Math::float3& position, float maxDist = FLT_MAX);

        /**
         * Plays the segment identified by a name
         */
        bool playSegment(const std::string& name, DirectMusic::SegmentTiming timing = DirectMusic::SegmentTiming::Immediate);

        /**
         * Plays the segment identified by the theme name
         */
        bool playMusicTheme(const std::string& name);

        /**
         * Returns the names of the currently loaded segments
         */
        std::vector<std::string> getLoadedSegments() const;

        /**
         * Stops all playing sounds
         */
        void stopSounds();

        /**
         * Pauses all sounds
         */
        void pauseSounds();

        /**
         * Continues all paused all sounds
         */
        void continueSounds();

    private:
        Engine::BaseEngine& m_Engine;

        /**
         * Pointer to a vdfs-index to work on (can be nullptr)
         */
        const VDFS::FileIndex& m_VDFSIndex;

        Daedalus::DaedalusVM* m_SoundVM = nullptr, *m_MusicVM = nullptr;

        struct Sound : public Handle::HandleTypeDescriptor<Handle::SfxHandle>
        {
            Daedalus::GEngineClasses::C_SFX sfx;
            std::vector<Handle::SfxHandle> variants;  // Instances ending with "_Ax"
            Audio::SoundPtr sound;
            std::string name;
        };

        void createSounds();

        Handle::SfxHandle allocateSound(const std::string& name, const Daedalus::GEngineClasses::C_SFX& sfx);

        void createSound(const std::string& name, const Daedalus::GEngineClasses::C_SFX& sfx);

        /**
         * Checks for sound-instancing ending in the same name, but with "_Ax" appended.
         * Adds those to the variants-list of the sound
         */
        void loadVariants(Handle::SfxHandle sfx);

        /**
         * Data allocator
         */
        Memory::StaticReferencedAllocator<Sound, Config::MAX_NUM_LEVEL_AUDIO_FILES> m_Allocator;

        /**
         * List of currently playing sounds or sounds that have been playing
         */
        std::list<Audio::SoundPtr> m_Sources;

        /**
         * Audio source for music
         * 
         */
        Audio::SoundPtr m_MusicSource;

        /**
         * Holds the music state
         */
        std::unique_ptr<DirectMusic::PlayingContext> m_musicContext;

        /**
         * Contains all the music segments that can be played in the instance
         */
        std::map<std::string, std::shared_ptr<DirectMusic::SegmentInfo>> m_Segments;

        /**
         * Convert a musictheme instance to segment name
         */
        std::map<std::string, std::string> m_musicThemeSegments;

        /**
         * Contains the name of the currently playing music segment
        */
        std::string m_playingSegment;

        /**
         * Used to signal when the music rendering thread should stop
        */
        bool m_exiting;

        /**
         * Music loading routine
         */
        void initializeMusic();

        /**
         * Contains all loaded sounds by name
         */
        std::map<std::string, Handle::SfxHandle> m_SoundMap;
    };
}
