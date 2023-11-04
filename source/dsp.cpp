#include "../dependencies/AudioFile.h"
#include "../dependencies/cppzmq/zmq.hpp"
#include <iostream>
#include <stdlib.h>

/* Naive approach to double audio length */
AudioFile<float>::AudioBuffer* time_stretch (AudioFile<float>::AudioBuffer *buf){
    
    return buf;
}

int main(int argc, char** argv){
    
    AudioFile<float> file;
    file.load("../wav_files/440Hz.wav");
    file.printSummary();

    AudioFile<float>::AudioBuffer buf;
    buf.resize(2);
    buf[0].resize(file.getNumSamplesPerChannel()*2);
    buf[1].resize(file.getNumSamplesPerChannel()*2);

    for(int i=0; i<file.getNumSamplesPerChannel()*2; i++){
        buf[0][i] = file.samples[0][i%file.getNumSamplesPerChannel()];
        buf[1][i] = file.samples[1][i%file.getNumSamplesPerChannel()];
    }

    file.setAudioBufferSize(2, file.getNumSamplesPerChannel()*2);
    bool writeSuccess = file.setAudioBuffer(buf);
    file.save("../Wav Files/result.wav", AudioFileFormat::Wave);

    return 1;
}