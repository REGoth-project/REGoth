#pragma once

#include <list>
#include <map>

#include <glm/glm.hpp>

#include <handle/HandleDef.h>
#include <memory/Config.h>
#include <vdfs/fileIndex.h>

typedef struct ALCcontext_struct ALCcontext;

namespace Audio
{

class AudioEngine;

}

namespace World
{

class AudioWorld
{
    friend class AudioEngine;

public:

    AudioWorld(Audio::AudioEngine &engine, const VDFS::FileIndex& vdfidx);

    virtual ~AudioWorld();

    /* Listener */

    void setListenerGain(float gain);

    void setListenerPosition(const glm::vec3 &position);

    void setListenerVelocity(const glm::vec3 &position);

    void setListenerOrientation(const glm::vec3 &at, const glm::vec3 &up);

    /**
     * @brief Loads an audio-file from the given or stored VDFS-FileIndex
     */
    Handle::AudioHandle loadAudioVDF(const VDFS::FileIndex& idx, const std::string& name);

    Handle::AudioHandle loadAudioVDF(const std::string& name);

    /**
     * Plays the sound of the given handle/name
     */
    void playSound(Handle::AudioHandle h);

    void playSound(const std::string& name);

    /**
     * Stops all playing sounds
     */
    void stopSounds();

private:

    ALCcontext *m_Context = NULL;

    struct Source
    {
        unsigned m_Handle = 0;
    };

    /**
     * Single audio-file
     */
    struct AudioFile : public Handle::HandleTypeDescriptor<Handle::AudioHandle>
    {
        unsigned m_Handle = 0;
    };

#ifdef RE_USE_SOUND
    /**
     * Checks if we currently have a stopped sound to use or creates a new one, if not
     * @return sound object to use for a new sound
     */
    Source getFreeSource();

    /**
     * Data allocator
     */
    Memory::StaticReferencedAllocator<AudioFile, Config::MAX_NUM_LEVEL_AUDIO_FILES> m_Allocator;

    /**
     * List of currently playing sounds or sounds that have been playing
     */
    std::list<Source> m_Sources;
#endif

    /**
     * Pointer to a vdfs-index to work on (can be nullptr)
     */
    const VDFS::FileIndex& m_VDFSIndex;

    /**
     * Contains all loaded sounds by name
     */
    std::map<std::string, Handle::AudioHandle> m_SoundMap;
private:

};
}
