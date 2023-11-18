//=================================================================================================
// Include Dependencies
//=================================================================================================
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

//=================================================================================================
// Standard Libraries
//=================================================================================================
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

//=================================================================================================
// Main
//=================================================================================================
int main(int argc, char** argv)
{
    // load file 
    AudioFile<float> file;
    file.load("../wav_files/sin_sweep.wav");
    file.printSummary();
    
    // create a new pipe using port 5500
    std::unique_ptr<UpstreamPipe> gui_pipe = std::make_unique<UpstreamPipe>(5500);
    
    std::cout << "connecting..." << std::endl;
    gui_pipe->connect(); 
    std::cout << "connected!" << std::endl;

    std::cout << "sending message..." << std::endl;
    std::cout << gui_pipe->send_message("test") << std::endl;
    std::cout << "message sent!" << std::endl;

    std::cout << "receiving message..." << std::endl;
    std::cout << "Message received: " << gui_pipe->recv_message() << std::endl;

    std::cout << "sending audio..." << std::endl;
    std::cout << gui_pipe->send_audio(&(file.samples)) << std::endl;
    std::cout << "audio sent!" << std::endl;

    return EXIT_SUCCESS;

    /*
    // set up connection to dsp
    asio::io_context io_context;
    std::shared_ptr<asio::ip::tcp::socket> socket_ptr(new asio::ip::tcp::socket(io_context));
    asio::ip::tcp::resolver resolver(io_context);
    asio::connect(*socket_ptr, resolver.resolve("localhost", "5500"));

    // load file 
    AudioFile<float> file;
    file.load("../wav_files/sin_sweep.wav");
    file.printSummary();
    
    // send command to dsp
    send_message(socket_ptr, "reverse");
    
    // send audio to dsp
    send_audio(socket_ptr, &(file.samples));
    
    // receive audio from dsp
    AudioFile<float>::AudioBuffer buf;
    buf.resize(2);
    recv_audio(socket_ptr, &buf);
    
    // close connection
    send_message(socket_ptr, "exit");
    socket_ptr->close();

    // put new audio in opened file and save as result.wav 
    file.setAudioBuffer(buf);
    file.printSummary();
    file.save("../wav_files/result.wav");
    
    return EXIT_SUCCESS;
    */
}