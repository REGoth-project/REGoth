#include <cstring>

#include <adpcm/adpcm-lib.h>

#include <utils/logger.h>

#include "WavReader.h"

WavReader::WavReader(const void *data, unsigned size)
    : m_Source(reinterpret_cast<const uint8_t*>(data)),
      m_SourceSize(size),
      m_SourceOffset(60)
{

}

bool WavReader::open()
{
    if (m_SourceSize < 60)
    {
        LogError() << "Wave file is too small";
        return false;
    }

    m_ADPCM = *reinterpret_cast<const uint16_t*>(&m_Source[20]) == 0x11;
    m_Channels = *reinterpret_cast<const uint16_t*>(&m_Source[22]);
    m_Rate = *reinterpret_cast<const uint32_t*>(&m_Source[24]);

    m_BlockSize = *reinterpret_cast<const uint16_t*>(&m_Source[32]);
    unsigned data_size = m_SourceSize - 60;

    if (m_ADPCM)
    {
        unsigned complete_blocks = data_size / m_BlockSize;
        unsigned samples_per_block = (m_BlockSize - m_Channels* 4) * (m_Channels ^ 3) + 1;
        m_SampleCount = complete_blocks * samples_per_block;
    } else
        m_SampleCount = data_size / m_BlockSize;


    return true;
}

int WavReader::read(unsigned max_samples)
{
    m_Dest.clear();

    if (m_ADPCM)
        return decodeADPCM(max_samples);

    unsigned left = m_SourceSize - m_SourceOffset;

    // 16 bit, 1 ch
    unsigned max_bytes = max_samples * 2;
    if (left > max_bytes)
        left = max_bytes;

    m_Dest.resize(left);
    memcpy(&m_Dest[0], &m_Source[m_SourceOffset], left);

    m_SourceOffset += left;
    return left;
}

int WavReader::decodeADPCM(unsigned max_samples)
{
    unsigned sample_count = getSampleCount();
    unsigned samples_per_block = (m_BlockSize - m_Channels * 4) * (m_Channels ^ 3) + 1;

    if (sample_count > max_samples)
        sample_count = max_samples;

    unsigned sample = 0;
    while (sample < sample_count) {
        int this_block_adpcm_samples = samples_per_block;
        int this_block_pcm_samples = samples_per_block;

        unsigned block_size = m_BlockSize;
        if (this_block_adpcm_samples > sample_count) {
            this_block_adpcm_samples = ((sample_count + 6) & ~7) + 1;
            block_size = (this_block_adpcm_samples - 1) / (m_Channels ^ 3) + (m_Channels * 4);
            this_block_pcm_samples = sample_count;
        }

        printf("offset: %d\n", m_SourceOffset);
        printf("block adpcm size: %d\n", this_block_adpcm_samples);
        printf("block pcm size: %d\n", this_block_pcm_samples);

        // Add to out-wav
        size_t ofidx = m_Dest.size();
        m_Dest.resize(m_Dest.size() + this_block_pcm_samples * m_Channels * 2);

        adpcm_decode_block(reinterpret_cast<int16_t*>(&m_Dest[ofidx]), &m_Source[m_SourceOffset], m_BlockSize, m_Channels);
        /*
        if (adpcm_decode_block(reinterpret_cast<int16_t*>(&dest[ofidx]), sourcep, block_size, num_channels) != this_block_adpcm_samples) {
            LogWarn() << "adpcm_decode_block() failed";
            return false;
        }
        */
        m_SourceOffset += m_BlockSize;
        sample += this_block_pcm_samples;
    }

    return sample;
}
