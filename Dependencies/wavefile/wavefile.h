#ifndef WAVEFILE_H_
#define WAVEFILE_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    
    /* --- RIFF Chunk Descriptor --- */
    char chunk_id[5];
    unsigned long chunk_size;
    char format[5];

    /* --- Format Subchunk --- */
    char subchunk1_id[4];
    unsigned int subchunk1_size;
    unsigned int audio_format;
    unsigned int num_channels;
    unsigned long sample_rate;
    unsigned long byte_rate;
    unsigned int block_align;
    unsigned int bit_depth;
    
    /* --- Data Subchunk --- */
    char subchunk2_id[5];
    unsigned long subchunk2_size;
    unsigned int sample_format;
    unsigned int channel_format;
    float* wave_data;

} WaveFile;

WaveFile* wavefile_create(unsigned int num_channels, 
                            unsigned long sample_rate, 
                            unsigned int sample_format, 
                            unsigned int channel_format);
WaveFile* wavefile_unpack_data(WaveFile* wavefile, float* data);
WaveFile* wavefile_pack_data(WaveFile* wavefile, float* data);
void* wavefile_destroy(WaveFile* file_to_destroy);
void wavefile_print(WaveFile* file);

#endif