/*
What is this file?
Structure
Dependencies
Data Structures
Functions
*/

#include <stdio.h>
#include <stdlib.h>
#include <types.h>

/*
##############################################################################
# Wave File Object
##############################################################################
This structure contains all the necessary information to construct and process
a .wav audio file.

Data structures and values must follow the format described here:
https://docs.fileformat.com/audio/wav/

*** SAMPLE RATE ***
The sample rate is a 32 byte positive integer representing how many times per
second the amplitude of the audio data is measured. Two samples are required
to define a wave, so the maximum frequency that can be represented in the
file is half the sample rate. The human hearing range extends up to 20-22kHz,
so a sample rate of at least 40kHz, often 44.1kHz by convention, is needed.

*** BIT DEPTH ***
The bit depth is an 8 bit positive integer representing the length in bits of
the numeric amplitude value of a wave. A greater bit depth means that the
amplitude of a wave can be captured in more detail. 16, 24, and 32 are common
bit depths for the .wav file format.

*** WAVE DATA ***
Wave data is a vector of float values that represents an audio waveform.
Successive indexes in the vector represent successive moments in time. The
float value stored in each index represents the amplitude of the audio wave
at that moment in time.
*/
struct WaveFile{
    ulong sample_rate;
    uint8 bit_depth;
    float* wave_data;
};

/*
##############################################################################
# Unpack Wave
##############################################################################
Retrieves data from an existing .wav file and returns a reference to a new
WaveFile object.
*/
WaveFile unpack_wav_file(FILE* wav_file){
}

/*
##############################################################################
# Send Raw Audio Data
##############################################################################
*/
void send_raw_audio_data(WaveFile* wav_file){
}

/*
##############################################################################
# Receive Raw Audio Data
##############################################################################
*/
float* recv_raw_audio_data(float* container){
    
}

/*
##############################################################################
# Main
##############################################################################

*/
int main(int argc, char** argv){
    /* open file */
    /* unpack audio data */
    /* send audio data to appropriate pipeline */
    /* pack received audio data */
    /* generate file */
}