/*=====================================================================================================================
== Include Dependencies
=====================================================================================================================*/
#include "..\dependencies\AudioFile\AudioFile.h"

//define windows version for asio (suppresses annoying warning)
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

//using standalone version of asio (as opposed to boost::asio)
#define ASIO_STANDALONE
#include "..\dependencies\asio\include\asio.hpp"

//defines protocols for sending and receiving audio through socket
#include "communication_protocols.hpp"

/*=====================================================================================================================
== Standard Libraries
=====================================================================================================================*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

/*=====================================================================================================================
== Audio Buffer Copying Functions
=====================================================================================================================*/
void buffer_copy_full(AudioFile<float>::AudioBuffer* original, AudioFile<float>::AudioBuffer* copy){

    // get the number of channels in the original buffer
    size_t num_channels = (*original).size();

    //copy contents of original buffer into copy
    for(size_t channel=0; channel<num_channels; channel++){
        //clear the audio vector to be copied into
        (*copy)[channel].clear();
        // get the number of samples in the original's channel
        size_t num_samples = (*original)[channel].size();
        //resize the copy channel's audio vector
        (*copy)[channel].resize(num_samples);
        //copy samples from original buffer into copy
        for(unsigned int sample=0; sample<num_samples; sample++){
            (*copy)[channel][sample] = (*original)[channel][sample];
        }
    }
}

void buffer_copy_part(AudioFile<float>::AudioBuffer* buf1, size_t buf1_idx,
                    AudioFile<float>::AudioBuffer* buf2, size_t buf2_idx, size_t num_samples){

    // get the number of channels in the original buffer
    size_t num_channels = (*buf1).size();
    size_t buf1_size = (*buf1)[0].size();
    size_t buf2_size = (*buf2)[0].size();

    // ensure both buffers are large enough
    if(buf1_size < (buf1_idx + num_samples)){
        buf1_size += (buf1_idx + num_samples) - buf1_size;
        (*buf1)[0].resize(buf1_size);
        (*buf1)[1].resize(buf1_size);
    }
    if(buf2_size < (buf2_idx + num_samples)){
        buf2_size += (buf2_idx + num_samples) - buf2_size;
        (*buf2)[0].resize(buf2_size);
        (*buf2)[1].resize(buf2_size); 
    }

    //copy contents of buf1 to buf2
    for(size_t channel=0; channel<num_channels; channel++){
        for(size_t sample=0; sample<num_samples; sample++){
            (*buf2)[channel][buf2_idx + sample] = (*buf1)[channel][buf1_idx + sample];
        }
    }
}

/*=====================================================================================================================
== Functions Relevant to SOLA (Synchronous Overlap Add)
=====================================================================================================================*/
void window_buffers(AudioFile<float>::AudioBuffer* buf1, AudioFile<float>::AudioBuffer* buf2){
    
    // get the number of channels in the first buffer
    size_t num_channels = (*buf1).size();
    size_t buf1_size = (*buf1)[0].size();
    size_t buf2_size = (*buf2)[0].size();
    
    // ensure buffers are equal length
    if(buf1_size < buf2_size){
        buf1_size = buf2_size;
        (*buf1)[0].resize(buf2_size);
        (*buf1)[1].resize(buf2_size);
    }
    else if(buf2_size < buf1_size){
        buf2_size = buf1_size;
        (*buf2)[0].resize(buf1_size);
        (*buf2)[1].resize(buf1_size);
    }
    size_t buf_size = buf1_size;

    //window and combine the temporary buffers
    for(size_t channel=0; channel<num_channels; channel++){
        for(size_t sample=0; sample<buf_size; sample++){
            (*buf1)[channel][sample] *= ((float)(buf_size-sample)/(float)(buf_size)); 
            (*buf2)[channel][sample] *= ((float)(sample)/(float)(buf_size));
            (*buf1)[channel][sample] += (*buf2)[channel][sample];
        }
    }
}

void sola(AudioFile<float>::AudioBuffer* input_samples, float time_scale){
    
    // independent values
    unsigned int frame_size = 4000;
    unsigned int overlap_size = 800;
    
    // dependent values
    unsigned int flat_duration = frame_size - (2 * overlap_size);
    unsigned int frame_skip = (int)((float)(frame_size - overlap_size) * (float)time_scale);

    // input output
    size_t num_input_samples = (*input_samples)[0].size();
    size_t num_channels = input_samples->size();
    size_t input_cursor = 0;

    // output buffer
    AudioFile<float>::AudioBuffer output_samples;
    output_samples.resize(num_channels);
    size_t output_cursor = 0;

    // divide audio into overlapping frames
    while(input_cursor <= num_input_samples - 2*(frame_skip + overlap_size)){

        // copy flat section to the output
        buffer_copy_part(input_samples, input_cursor, &output_samples, output_cursor, flat_duration);

        // move cursors
        output_cursor += flat_duration;
        input_cursor += flat_duration;
        
        // temporary buffer holds current frame's overlap tail
        AudioFile<float>::AudioBuffer prev_frame_tail;
        prev_frame_tail.resize(num_channels);
        buffer_copy_part(input_samples, input_cursor, &prev_frame_tail, 0, overlap_size);
        
        // temporary buffer holds next frame's overlap head
        AudioFile<float>::AudioBuffer next_frame_head;
        next_frame_head.resize(num_channels);
        buffer_copy_part(input_samples, (input_cursor + frame_skip - overlap_size), &next_frame_head, 0, overlap_size);

        // move cursor to next frame's flat section
        input_cursor += frame_skip;

        // window and combine the overlap buffers
        window_buffers(&prev_frame_tail, &next_frame_head);
        
        // add the overlap to the output buffer
        buffer_copy_part(&prev_frame_tail, 0, &output_samples, output_cursor, overlap_size);
        output_cursor += overlap_size;
    }
    // copy the full output to the input container and return
    buffer_copy_full(&output_samples, input_samples);
    return;
}

/*=====================================================================================================================
== Pitch Shift
=====================================================================================================================*/
void pitch_shift(std::shared_ptr<asio::ip::tcp::socket> socket){
    
}
/*=====================================================================================================================
== Time Stretch
=====================================================================================================================*/
void time_stretch(std::shared_ptr<asio::ip::tcp::socket> socket){
    AudioFile<float>::AudioBuffer buffer; 
    buffer.resize(2);
    
    recv_audio(socket, &buffer);
    
    sola(&buffer, (float)0.5);
    
    send_audio(socket, &buffer);
}
/*=====================================================================================================================
== Reverse
=====================================================================================================================*/
void reverse(std::shared_ptr<asio::ip::tcp::socket> socket){
    
    AudioFile<float>::AudioBuffer buffer; 
    buffer.resize(2);
    
    recv_audio(socket, &buffer);

    //for each audio channel
    for(int i=0; i<buffer.size(); i++){
        size_t num_samples = buffer[i].size();
        // for each sample in an audio channel
        for(int j=0; j<(num_samples - j - 1); j++){
            // swap two samples
            float temp = buffer[i][j];
            buffer[i][j] = buffer[i][num_samples - j - 1];
            buffer[i][num_samples - j - 1] = temp;
        }
    }

    send_audio(socket, &buffer);
}
/*=====================================================================================================================
== Main
=====================================================================================================================*/
int main(int argc, char** argv){

    // initialize context and socket
    asio::io_context io_context;
    std::shared_ptr<asio::ip::tcp::socket> socket_ptr(new asio::ip::tcp::socket(io_context));
    
    // acceptor
    std::shared_ptr<asio::ip::tcp::acceptor> acceptor_ptr
        (new asio::ip::tcp::acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 13)));
    (*acceptor_ptr).accept(*socket_ptr);

    while(1){
        std::string receiver;
        recv_message(socket_ptr, &receiver);

        if(receiver.compare("pitchshift") == 0){
            pitch_shift(socket_ptr);
        }
        else if(receiver.compare("timestretch") == 0){
            time_stretch(socket_ptr);
        }
        else if(receiver.compare("reverse") == 0){
            reverse(socket_ptr);
        }
        else if(receiver.compare("exit") == 0){
            return 0;
        }
    }
}