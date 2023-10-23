#include "wavefile.h"

/*
    --- RIFF Chunk Descriptor --- 
    char chunk_id[5];
    unsigned long chunk_size;
    char format[5];

    --- Format Subchunk --- 
    char subchunk1_id[4];
    unsigned int subchunk1_size;
    unsigned int audio_format;
    unsigned int num_channels;
    unsigned long sample_rate;
    unsigned long byte_rate;
    unsigned int block_align;
    uint8_t bit_depth;
    
    --- Data Subchunk --- 
    char subchunk2_id[5];
    unsigned long subchunk2_size;
    float* wave_data;

} WaveFile;
*/

WaveFile* wavefile_create(){

    WaveFile* new_wavefile = malloc(sizeof(WaveFile));

    /* --- RIFF Chunk Descriptor --- */
    strcpy(new_wavefile->chunk_id, "RIFF");
    new_wavefile->chunk_size = 12 + 16 + 8; /* + audio data size*/
    strcpy(new_wavefile->format, "WAVE");
    
    /* --- Format Subchunk --- */
    strcpy(new_wavefile->subchunk1_id, "fmt");
    new_wavefile->subchunk1_size = 16;
    new_wavefile->audio_format = 1;
    new_wavefile->num_channels = 1;
    new_wavefile->sample_rate = 44100;
    new_wavefile->byte_rate = 176400;
    new_wavefile->block_align = 4;
    new_wavefile->bit_depth = 16;

    /* --- Data Subchunk --- */
    strcpy(new_wavefile->subchunk2_id, "data");
    new_wavefile->subchunk2_size = 8;
    new_wavefile->wave_data = malloc(sizeof(float*));

    return new_wavefile;
}

WaveFile* wavefile_unpack_data(WaveFile* wavefile, int* data){
    //unpack raw audio data from wavefile and place it in data
}

WaveFile* wavefile_pack_data(WaveFile* wavefile, int* data){
    //pack raw audio data from data into wavefile
}

void* wavefile_destroy(WaveFile* file_to_destroy){

    free(file_to_destroy->wave_data);
    free(file_to_destroy);

    return file_to_destroy;
}