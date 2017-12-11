#pragma once

#include <list>
#include <map>
#include <thread>

#include <glm/glm.hpp>

#include <daedalus/DaedalusStdlib.h>
#include <handle/HandleDef.h>
#include <memory/Config.h>
#include <utils/Utils.h>
#include <vdfs/fileIndex.h>

typedef struct ALCcontext_struct ALCcontext;

#ifdef RE_USE_SOUND
#define RE_NUM_MUSIC_BUFFERS 3
#define RE_MUSIC_BUFFER_LEN 1024
#endif

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

namespace DirectMusic
{
    class PlayingContext;
    class SegmentInfo;
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

        /* Listener (TODO) */

        void setListenerGain(float gain);

        void setListenerPosition(const Math::float3& position);

        void setListenerVelocity(const Math::float3& velocity);

        void setListenerOrientation(const Math::float3& at, const Math::float3& up);

        /**
         * @brief Loads an audio-file from the given or stored VDFS-FileIndex
         */
        Handle::SfxHandle loadAudioVDF(const VDFS::FileIndex& idx, const std::string& name);

        Handle::SfxHandle loadAudioVDF(const std::string& name);

        /**
         * Plays the sound of the given handle/name
         */
        Utils::Ticket<AudioWorld> playSound(Handle::SfxHandle h, const Math::float3& position, bool relative, float maxDist = FLT_MAX);
        Utils::Ticket<AudioWorld> playSound(Handle::SfxHandle h);
        Utils::Ticket<AudioWorld> playSound(const std::string& name);
        Utils::Ticket<AudioWorld> playSoundVariantRandom(const std::string& name);
        Utils::Ticket<AudioWorld> playSoundVariantRandom(Handle::SfxHandle h);

        Utils::Ticket<AudioWorld> playSound(Handle::SfxHandle h, const Math::float3& position, float maxDist = FLT_MAX);
        Utils::Ticket<AudioWorld> playSound(const std::string& name, const Math::float3& position, float maxDist = FLT_MAX);
        Utils::Ticket<AudioWorld> playSoundVariantRandom(const std::string& name, const Math::float3& position, float maxDist = FLT_MAX);
        Utils::Ticket<AudioWorld> playSoundVariantRandom(Handle::SfxHandle h, const Math::float3& position, float maxDist = FLT_MAX);

        /**
         * Plays the segment identified by a name
         */
        bool playSegment(const std::string& name);

        /**
         * Returns the names of the currently loaded segments
         */
        const std::vector<std::string> getLoadedSegments() const;

        /**
         * Sets the maximum distance this sound can be heard
         * @param maxDist Distance in meters
         */
        void setSoundMaxDistance(Utils::Ticket<AudioWorld> sound, float maxDist);

        /**
         * Stops all playing sounds
         */
        void stopSounds();

        /**
         * Stops Sound with given Ticket
         * @param ticket to identify the sound
         */
        void stopSound(Utils::Ticket<AudioWorld> ticket);

        /**
         * returns whether the source of the associated ticket is playing
         */
        bool soundIsPlaying(Utils::Ticket<AudioWorld> ticket);

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

        ALCcontext* m_Context = nullptr;

        Daedalus::DaedalusVM* m_VM = nullptr;

        struct Source
        {
            unsigned m_Handle = 0;
            Utils::Ticket<AudioWorld> soundTicket;
        };

        struct Sound : public Handle::HandleTypeDescriptor<Handle::SfxHandle>
        {
            Daedalus::GEngineClasses::C_SFX sfx;
            std::vector<Handle::SfxHandle> variants;  // Instances ending with "_Ax"
            unsigned m_Handle = 0;
            std::string name;
        };

#ifdef RE_USE_SOUND

        void createSounds();

        Handle::SfxHandle allocateSound(const std::string& name, const Daedalus::GEngineClasses::C_SFX& sfx);

        void createSound(const std::string& name, const Daedalus::GEngineClasses::C_SFX& sfx);

        /**
         * Checks for sound-instancing ending in the same name, but with "_Ax" appended.
         * Adds those to the variants-list of the sound
         */
        void loadVariants(Handle::SfxHandle sfx);

        /**
         * Checks if we currently have a stopped sound to use or creates a new one, if not
         * @return sound object to use for a new sound
         */
        Source getFreeSource();

        /**
         * Data allocator
         */
        Memory::StaticReferencedAllocator<Sound, Config::MAX_NUM_LEVEL_AUDIO_FILES> m_Allocator;

        /**
         * List of currently playing sounds or sounds that have been playing
         */
        std::list<Source> m_Sources;

        /**
         * Holds the music state
         */
        std::unique_ptr<DirectMusic::PlayingContext> m_musicContext;

        /**
         * Contains all the music segments that can be played in the instance
         */
        std::map<std::string, std::shared_ptr<DirectMusic::SegmentInfo>> m_Segments;

        /**
         * Background thread that puts music data into the soundbuffer(s)
         */
        void musicRenderFunction();
        std::thread m_musicRenderThread;

        /**
         * Contain music buffers and source
         */
        unsigned m_musicBuffers[RE_NUM_MUSIC_BUFFERS], m_musicSource;

        /**
         * Used to signal when the music rendering thread should stop
        */
        bool m_exiting;

        /**
         * Music loading routine
         */
        void initializeMusic();
#endif

        /**
         * Contains all loaded sounds by name
         */
        std::map<std::string, Handle::SfxHandle> m_SoundMap;
    };
}
