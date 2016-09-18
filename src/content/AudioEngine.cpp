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

Handle::AudioHandle AudioEngine::loadAudioVDF(const VDFS::FileIndex& idx, const std::string& name)
{
    std::vector<uint8_t> data;

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

    // Decode the ADPCM compressed audio gothic uses
    
    //
    // ###### This is broken! Decode using new lib, load samples directly into sfml (44100 khz, 1 channel)
    //
    
    /*static bool s_initDone = false;
    if(!s_initDone)
    {
        initDecode68000();
        s_initDone = true;
    }
    
    std::vector<uint8_t> uncompressedWav;
    decodeWAV(data, uncompressedWav);
    
    
    // Load the buffer with the data from the VDF
    if(!a.buffer.loadFromMemory(uncompressedWav.data(), uncompressedWav.size()))
    {      
        m_Allocator.removeObject(h);
        return Handle::AudioHandle::makeInvalidHandle();
    }*/

    m_SoundMap[name] = h;

    return h;
}

Handle::AudioHandle AudioEngine::loadAudioVDF(const std::string& name)
{
    if(!m_pVDFSIndex)
        return Handle::AudioHandle::makeInvalidHandle();

    return loadAudioVDF(*m_pVDFSIndex, name);
}

void AudioEngine::playSound(Handle::AudioHandle h)
{
    AudioFile& a = m_Allocator.getElement(h);

    // Get a cached sound-object
    sf::Sound& s = getFreeSoundObject();
    s.setBuffer(a.buffer);

    // Play our sound. If this runs out, we can later use the sf::Sound again, for an other buffer
    s.play();
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
