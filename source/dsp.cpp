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
== Pitch Shift
=====================================================================================================================*/
void pitch_shift(std::shared_ptr<asio::ip::tcp::socket> socket){
    
}
/*=====================================================================================================================
== Time Stretch
=====================================================================================================================*/
void time_stretch(std::shared_ptr<asio::ip::tcp::socket> socket){

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
            return 0;
        }
        else if(receiver.compare("exit") == 0){
            return 0;
        }
    }

    return 0;
}