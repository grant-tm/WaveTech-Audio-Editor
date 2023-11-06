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

    /* initialize context and socket */
    asio::io_context io_context;
    std::shared_ptr<asio::ip::tcp::socket> socket_ptr(new asio::ip::tcp::socket(io_context));
    
    /* acceptor */
    std::shared_ptr<asio::ip::tcp::acceptor> acceptor_ptr
        (new asio::ip::tcp::acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 13)));
    (*acceptor_ptr).accept(*socket_ptr);

    AudioFile<float>::AudioBuffer new_buffer; 
    new_buffer.resize(2);
    
    recv_audio(socket_ptr, &new_buffer);
    send_audio(socket_ptr, &new_buffer);

    return 0;
}