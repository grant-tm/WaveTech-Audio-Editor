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
== Main
=====================================================================================================================*/
int main(int argc, char** argv){
    
    /* client-server setup */
    asio::io_context io_context;
    /* resolve endpoint */
    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::resolver::results_type endpoint = resolver.resolve("localhost", "daytime");
    /* open socket and connect to endpoint */
    std::shared_ptr<asio::ip::tcp::socket> socket_ptr(new asio::ip::tcp::socket(io_context));
    asio::connect(*socket_ptr, endpoint);

    /* load a file */
    AudioFile<float> file;
    file.load("../wav_files/sin_sweep.wav");
    file.printSummary();
    /* create new audio buffer */
    AudioFile<float>::AudioBuffer buf;
    buf.resize(2);

    /* send audio to dsp */
    bool result = send_audio(socket_ptr, &(file.samples));
    /* receive audio from dsp */
    recv_audio(socket_ptr, &buf);

    /* put new audio in opened file and save as result.wav */
    file.setAudioBuffer(buf);
    file.printSummary();
    file.save("../wav_files/result.wav");
    
    return 0;
}