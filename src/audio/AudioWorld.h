#pragma once

#include <list>
#include <map>

#include <glm/glm.hpp>

#include <handle/HandleDef.h>
#include <memory/Config.h>
#include <vdfs/fileIndex.h>
#include <utils/Utils.h>

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

    AudioWorld(Engine::BaseEngine &engine, Audio::AudioEngine &audio_engine, const VDFS::FileIndex& vdfidx);

    virtual ~AudioWorld();

    /* Listener (TODO) */

    void setListenerGain(float gain);

    void setListenerPosition(const glm::vec3 &position);

    void setListenerVelocity(const glm::vec3 &position);

    void setListenerOrientation(const glm::vec3 &at, const glm::vec3 &up);

    /**
     * @brief Loads an audio-file from the given or stored VDFS-FileIndex
     */
    Handle::SfxHandle loadAudioVDF(const VDFS::FileIndex& idx, const std::string& name);

    Handle::SfxHandle loadAudioVDF(const std::string& name);

    /**
     * Plays the sound of the given handle/name
     */
    Utils::Ticket<AudioWorld> playSound(Handle::SfxHandle h);

    Utils::Ticket<AudioWorld> playSound(const std::string& name);

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

private:

    Engine::BaseEngine &m_Engine;

    /**
     * Pointer to a vdfs-index to work on (can be nullptr)
     */
    const VDFS::FileIndex& m_VDFSIndex;

    ALCcontext *m_Context = nullptr;

    Daedalus::DaedalusVM *m_VM = nullptr;

    struct Source
    {
        unsigned m_Handle = 0;
        Utils::Ticket<AudioWorld> soundTicket;
    };

    struct Sound : public Handle::HandleTypeDescriptor<Handle::SfxHandle>
    {
        Daedalus::GEngineClasses::C_SFX sfx;
        unsigned m_Handle = 0;
    };

#ifdef RE_USE_SOUND

    void createSounds();

    Handle::SfxHandle allocateSound(const std::string &name, const Daedalus::GEngineClasses::C_SFX &sfx);

    void createSound(const std::string &name, const Daedalus::GEngineClasses::C_SFX &sfx);

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
#endif

    /**
     * Contains all loaded sounds by name
     */
    std::map<std::string, Handle::SfxHandle> m_SoundMap;
};
}
