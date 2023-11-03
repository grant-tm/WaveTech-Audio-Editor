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
    unsigned int bit_depth;
    
    --- Data Subchunk --- 
    char subchunk2_id[5];
    unsigned long subchunk2_size;
    float* wave_data;

} WaveFile;
*/

WaveFile* wavefile_create(unsigned int num_channels, 
                            unsigned long sample_rate, 
                            unsigned int sample_format, 
                            unsigned int channel_format){

    WaveFile* new_wavefile = (WaveFile*) malloc(sizeof(WaveFile));

    /* --- Fill in RIFF Chunk Descriptor --- */
    strcpy(new_wavefile->chunk_id, "RIFF");
    new_wavefile->chunk_size = 0; /* fill in later */
    strcpy(new_wavefile->format, "WAVE");
    
    /* --- Fill in Format Subchunk --- */
    strcpy(new_wavefile->subchunk1_id, "fmt");
    new_wavefile->subchunk1_size = 16;
    new_wavefile->audio_format = 1;
    new_wavefile->num_channels = num_channels;
    new_wavefile->sample_rate = sample_rate;
    new_wavefile->byte_rate = sample_rate * num_channels * sample_format;
    new_wavefile->block_align = num_channels * sample_format;
    new_wavefile->bit_depth = 8 * sample_format;

    /* --- Fill in Data Subchunk --- */
    strcpy(new_wavefile->subchunk2_id, "data");
    new_wavefile->subchunk2_size = 0; /* fill in later */
    new_wavefile->wave_data = (float*) malloc(sizeof(float*));

    return new_wavefile;
}

WaveFile* wavefile_unpack_data(WaveFile* wavefile, float* data){
    //unpack raw audio data from wavefile and place it in data
}

WaveFile* wavefile_pack_data(WaveFile* wavefile, float* data){
    //pack raw audio data from data into wavefile
    wavefile->wave_data = (float*) realloc(wavefile->wave_data, sizeof(data));
    for(unsigned int i=0; i<sizeof(data)*2; i++){
        wavefile->wave_data[i] = data[i];
    }
    return wavefile;
}

void* wavefile_destroy(WaveFile* file_to_destroy){
    free(file_to_destroy->wave_data);
    free(file_to_destroy);
    return file_to_destroy;
}

void wavefile_print(WaveFile* file){
    printf("chunk_id: %s\n", file->chunk_id);
    printf("chunk_size: %lu\n", file->chunk_size);
    printf("format: %s\n", file->format); 
    printf("subchunk1_id: %s\n", file->subchunk1_id);
    printf("subchunk1_size: %i\n", file->subchunk1_size);
    printf("audio_format: %i\n", file->audio_format);
    printf("num_channels: %i\n", file->num_channels);
    printf("sample_rate: %lu\n", file->sample_rate);
    printf("byte_rate: %lu\n", file->byte_rate);
    printf("block_align: %i\n", file->block_align);
    printf("bit_depth: %i\n", file->bit_depth);
    printf("subchunk2_id: %s\n", file->subchunk2_id);
    printf("subchunk2_size: %lu\n", file->subchunk2_size);
    printf("wave_data: [");
    for(unsigned int i=0; i<sizeof(file->wave_data)*2; i++){
        printf("%f,", file->wave_data[i]);
    }
    printf("]\n");
    return;
}