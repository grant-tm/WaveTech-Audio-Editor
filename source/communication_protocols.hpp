#ifndef _COMMUNICATION_PROTOCOLS
#define _COMMUNICATION_PROTOCOLS

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

/*=====================================================================================================================
== Standard Libraries
=====================================================================================================================*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

/*=====================================================================================================================
== generic function for confirmation checking
=====================================================================================================================*/
/*
bool recv_confirmation(std::shared_ptr<asio::ip::tcp::socket> socket, std::string confirmation_message){
    
    std::string message_received;
    socket->read_some(asio::buffer(message_received));
    
    if(message_received == confirmation_message){
        return true;
    }
    else{
        return false;
    }
}
*/

/*=====================================================================================================================
== generic function for confirmation checking
=====================================================================================================================*/
/*
bool send_confirmation(std::shared_ptr<asio::ip::tcp::socket> socket, std::string confirmation_message){
    asio::error_code ignored_error;
    asio::write((*socket), asio::buffer(confirmation_message), ignored_error);
    return true;
}

/*=====================================================================================================================
== Send Audio Data
=====================================================================================================================*/
bool send_audio(std::shared_ptr<asio::ip::tcp::socket> socket,
                asio::ip::tcp::resolver::results_type endpoint,
                AudioFile<float>::AudioBuffer* samples){

    asio::connect(*socket, endpoint);
    asio::error_code ignored_error;
    
    asio::write(*socket, asio::buffer((*samples)[0]), ignored_error);
    asio::write(*socket, asio::buffer((*samples)[1]), ignored_error);
    //recv_confirmation(socket, "received left channel");
    
    //asio::write(socket, asio::buffer(samples[1]), ignored_error);
    //recv_confirmation(socket, "received right channel");
    
    return true;
}
/*=====================================================================================================================
== Receive Audio Data
=====================================================================================================================*/
void recv_audio(std::shared_ptr<asio::ip::tcp::socket> socket,
                std::shared_ptr<asio::ip::tcp::acceptor> acceptor,
                AudioFile<float>::AudioBuffer* container){
    
    (*acceptor).accept(*socket);
    std::vector<float> recv;
    
    /* get left channel data */
    recv.resize(5513);
    (*socket).read_some(asio::buffer(recv));
    
    for(int i=0; i<recv.size(); i++){
        (*container)[0][i] = recv[i];
        std::cout << recv[i] << std::endl;
    }
    
    /* get right channel data */
    recv.clear();
    (*socket).read_some(asio::buffer(recv));
    
    for(int i=0; i<recv.size(); i++){
        (*container)[1][i] = recv[i];
        std::cout << recv[i] << std::endl;
    }

    AudioFile<float> new_file;
    new_file.load("../wav_files/Short_440Hz.wav");
    new_file.setAudioBuffer(*container);
    new_file.save("../wav_files/result.wav");

}

#endif