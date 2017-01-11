//
// Created by andre on 03.09.16.
//

#include <utils/logger.h>
#include "AudioEngine.h"
#include <adpcm/adpcm-lib.h>

using namespace Content;

AudioEngine::AudioEngine(const VDFS::FileIndex* vdfidx) :
    m_pVDFSIndex(vdfidx)
{

}

AudioEngine::~AudioEngine()
{

}

int AudioEngine::adpcm_decode_data(uint8_t* infile, std::vector<uint8_t>& outfile, size_t num_samples, int num_channels , int block_size )
{
	int samples_per_block = (block_size - num_channels * 4) * (num_channels ^ 3) + 1, percent;
	int16_t *pcm_block;
	uint8_t *adpcm_block;
	size_t progress_divider = 0;


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

Handle::AudioHandle AudioEngine::loadAudioVDF(const VDFS::FileIndex& idx, const std::string& name)
{
#ifdef RE_USE_SFML 
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
    
    // Create the buffer-object
    Handle::AudioHandle h = m_Allocator.createObject();
    AudioFile& a = m_Allocator.getElement(h);

    const uint8_t *formatPtr = data.data() + 20;
    const uint16_t audioFormat = *reinterpret_cast<const uint16_t *>(formatPtr);

    LogInfo() << "Audio format: " << audioFormat;

    bool loaded = false;
    if (audioFormat == 0x11) // ADPCM
    {
        // Decode the ADPCM compressed audio gothic uses

        // Gothics wav-files have a headersize of 60 bytes, 1 channel, blocksize of 1024
        size_t numNibbles = (data.size() - 60) * 2;
        adpcm_decode_data(data.data()+60, outData, numNibbles); // num channels, blocksize 1024

        loaded = a.buffer.loadFromSamples(reinterpret_cast<sf::Int16*>(outData.data()), (outData.size()/2) - 512 , 1,  44100);
    } else {
        // Fallback to SFML default load method
        loaded = a.buffer.loadFromMemory(data.data(), data.size());
    }
    if (!loaded)
    {
        m_Allocator.removeObject(h);
        return Handle::AudioHandle::makeInvalidHandle();
    }

    m_SoundMap[name] = h;

    return h;
#else
    return Handle::AudioHandle::makeInvalidHandle();
#endif
}

Handle::AudioHandle AudioEngine::loadAudioVDF(const std::string& name)
{
    if(!m_pVDFSIndex)
        return Handle::AudioHandle::makeInvalidHandle();

    return loadAudioVDF(*m_pVDFSIndex, name);
}

void AudioEngine::playSound(Handle::AudioHandle h)
{
#ifdef RE_USE_SFML 
    AudioFile& a = m_Allocator.getElement(h);

    // Get a cached sound-object
    sf::Sound& s = getFreeSoundObject();
    s.setBuffer(a.buffer);

    // Play our sound. If this runs out, we can later use the sf::Sound again, for an other buffer
    s.play();
#endif
}

void AudioEngine::playSound(const std::string& name)
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

#ifdef RE_USE_SFML 
sf::Sound& AudioEngine::getFreeSoundObject()
{
    // Check if we could re-use one
    for(sf::Sound& s : m_PlayingSounds)
    {
        if(s.getStatus() == sf::SoundSource::Stopped)
            return s;
    }

    // Nothing to re-use available, make a new entry
    m_PlayingSounds.push_back(sf::Sound());
    return m_PlayingSounds.back();
}
#endif

void AudioEngine::stopSounds()
{

#ifdef RE_USE_SFML 
    for(sf::Sound& s : m_PlayingSounds)
    {
        s.stop();
    }
#endif
}

