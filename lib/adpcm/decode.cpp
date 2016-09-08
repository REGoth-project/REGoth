#include <iostream>
#include <stdio.h>
#include <vector>
#include "adpcm-lib.h"

static int adpcm_decode_data (uint8_t* infile, std::vector<uint8_t>& outfile, size_t num_samples, int num_channels = 1, int block_size = 1024)
{
    int samples_per_block = (block_size - num_channels * 4) * (num_channels ^ 3) + 1, percent;
    int16_t *pcm_block;
    uint8_t *adpcm_block;
    size_t progress_divider = 0;

    if (!pcm_block || !adpcm_block) {
        fprintf (stderr, "could not allocate memory for buffers!\n");
        return -1;
    }

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
        
        if (adpcm_decode_block (pcm_block, adpcm_block, block_size, num_channels) != this_block_adpcm_samples) {
            fprintf (stderr, "adpcm_decode_block() did not return expected value!\n");
            return -1;
        }

        num_samples -= this_block_pcm_samples;
    }

    return 0;
}

int main(int argc, char** argv)
{
    std::vector<uint8_t> inwav;
    std::vector<uint8_t> outwav;
    
    // Read wav-file into memory
    FILE* f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    size_t s = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    inwav.resize(s);
    fread(inwav.data(), s, 1, f);
    
    fclose(f);
    
    // Gothics wav-files have a headersize of 60 bytes, 1 channel, blocksize of 1024
    size_t numNibbles = (inwav.size() - 60) * 2;
    adpcm_decode_data(inwav.data() + 60, outwav, numNibbles);
    
    f = fopen("out.wav", "wb");
    fwrite(outwav.data(), outwav.size(), 1, f);
    fclose(f);
    
    return 0;
}
