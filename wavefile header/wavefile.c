#include "wavefile.h"

WaveFile* wavefile_create(){

    WaveFile* new_wavefile = malloc(sizeof(WaveFile));
    
    /* initialize values */
    
    return new_wavefile;
}

WaveFile* wavefile_unpack_data(WaveFile* wavefile, int* data);

WaveFile* wavefile_pack_data(WaveFile* wavefile, int* data);

void* wavefile_destroy(WaveFile* file_to_destroy){

    /* free data memory */
    /* free file_to_destroy */

    return file_to_destroy;
}