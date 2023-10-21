#ifndef WAVEFILE_H_
#define WAVEFILE_H_

#include <stdint.h>

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
    uint8_t bit_depth;
    
    /* --- Data Subchunk --- */
    char subchunk2_id[5];
    unsigned long subchunk2_size;
    float* wave_data;

} WaveFile;

WaveFile* wavefile_create();
WaveFile* wavefile_unpack_data(WaveFile* wavefile, int* data);
WaveFile* wavefile_pack_data(WaveFile* wavefile, int* data);
void* wavefile_destroy(WaveFile* file_to_destroy);

#endif