#ifndef WAVREADER_H
#define WAVREADER_H

#include <cstdint>
#include <vector>

/** A class which reads ADPCM / raw .wav files.
 *
 */
class WavReader
{
public:

    /** Constructs the .wav reader.
     * @brief WavReader
     * @param data
     */
    WavReader(const void *data, unsigned size);

    bool open();

    unsigned getChannels() const { return m_Channels; }

    unsigned getRate() const { return m_Rate; }

    unsigned getBlockSize() const { return m_BlockSize; }

    unsigned getSampleCount() const { return m_SampleCount; }


    /** Reads all data from the wav.
     *
     * @return Returns the number of bytes read or < 0.
     *
     */
    int read() { return read(-1); }

    int read(unsigned max_samples);

    /** Returns the current block of decoded data.
     *
     * Call read() before accessing data.
     *
     * @return The currently decoded block.
     *
     */
    const void *getData() const { return m_Dest.empty() ? nullptr : &m_Dest[0]; }

    unsigned getDataSize() const { return m_Dest.size(); }

private:

    const uint8_t *m_Source = nullptr;
    unsigned m_SourceSize = 0;
    unsigned m_SourceOffset = 0;

    std::vector<uint8_t> m_Dest;

    bool m_ADPCM = false;
    uint16_t m_Channels = 0;
    uint32_t m_Rate = 0;
    uint16_t m_BlockSize = 0;

    unsigned m_SampleCount= 0;

    int decodeADPCM(unsigned max_samples);
};

#endif // WAVREADER_H
