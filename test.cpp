#include "Dependencies/AudioFile.h"
#include <iostream>
#include <stdlib.h>

int main(int argc, char** argv){
    
    AudioFile<float> file;
    file.load("Wav Files/440Hz.wav");
    file.printSummary();

    return 1;
}