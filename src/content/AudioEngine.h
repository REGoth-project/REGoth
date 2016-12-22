#pragma once

#include <memory/Config.h>
#include <memory/StaticReferencedAllocator.h>
#include <vdfs/fileIndex.h>
#include <handle/HandleDef.h>
#include <handle/Handle.h>
#include <map>
#include <list>

#ifdef RE_USE_SFML 
#include <SFML/Audio.hpp>
#endif

namespace Content
{
    /**
     * Simple audio-engine, wrapping SFML
     */
    class AudioEngine
    {
    public:
        AudioEngine(const VDFS::FileIndex* vdfidx = nullptr);
        virtual ~AudioEngine();

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

        /**
        * @brief Sets the VDFS-Index to use
        */
        void setVDFSIndex(const VDFS::FileIndex* vdfidx) { m_pVDFSIndex = vdfidx; }
    private:

        /**
         * Decode a IMA ADPCM-Wave file
         */
        int adpcm_decode_data(uint8_t* infile, std::vector<uint8_t>& outfile, size_t num_samples, int num_channels = 1, int block_size = 1024);
        
#ifdef RE_USE_SFML
        /**
         * Checks if we currently have a stopped sound to use or creates a new one, if not
         * @return sound object to use for a new sound
         */
        sf::Sound& getFreeSoundObject();

        /**
         * Single audio-file
         */
        struct AudioFile : public Handle::HandleTypeDescriptor<Handle::AudioHandle>
        {

            sf::SoundBuffer buffer;
        };

        /**
		 * Data allocator
		 */
        Memory::StaticReferencedAllocator<AudioFile, Config::MAX_NUM_LEVEL_AUDIO_FILES> m_Allocator;

        /**
         * List of currently playing sounds or sounds that have been playing
         */
        std::list<sf::Sound> m_PlayingSounds;
#endif
        
        /**
         * Pointer to a vdfs-index to work on (can be nullptr)
         */
        const VDFS::FileIndex* m_pVDFSIndex;

        /**
         * Contains all loaded sounds by name
         */
        std::map<std::string, Handle::AudioHandle> m_SoundMap;
    };
}
