#include "wavefile/wavefile.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
    
    WaveFile* w = wavefile_create(1, 44100, 1, 0);
    float audio_data[16];
    for(int i=0; i<16; i++){
        audio_data[i] = (float) i * 0.5;
    }
    for(int i=0; i<16; i++){
        printf("test: %f\n", audio_data[i]);
    }

    wavefile_pack_data(w, audio_data);

    wavefile_print(w);

    wavefile_destroy(w);

    return 0;
}