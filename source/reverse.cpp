/*=====================================================================================================================
== Include Dependencies
=====================================================================================================================*/
#include "..\dependencies\AudioFile\AudioFile.h"

//defines protocols for sending and receiving audio through socket
#include "Pipe.hpp"

/*=====================================================================================================================
== Standard Libraries
=====================================================================================================================*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

/*=====================================================================================================================
== Reverse
=====================================================================================================================*/
AudioFile<float>::AudioBuffer* reverse(AudioFile<float>::AudioBuffer* buffer)
{
    size_t num_channels = (*buffer).size();
    for(size_t channel=0; channel<num_channels; channel++)
    {
       std::reverse((*buffer)[channel].begin(), (*buffer)[channel].end());
    }
    return buffer;
}
/*=====================================================================================================================
== Main
=====================================================================================================================*/
int main(int argc, char** argv)
{
    /*
    // initialize context and socket
    asio::io_context io_context;
    std::shared_ptr<asio::ip::tcp::socket> dsp_connection_socket(new asio::ip::tcp::socket(io_context));
    // acceptor
    std::shared_ptr<asio::ip::tcp::acceptor> acceptor_ptr
        (new asio::ip::tcp::acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8800)));
    
    while(1)
    {
        (*acceptor_ptr).accept(*dsp_connection_socket);
        std::string receiver;
        recv_message(dsp_connection_socket, &receiver);
        std::cout << "message received: " << receiver << std::endl;
        
        if(receiver.compare("run") == 0)
        {
            // create new audio buffer
            AudioFile<float>::AudioBuffer buf;
            buf.resize(2);
            
            // handshake with dsp
            send_message(dsp_connection_socket, "ready");

            // receive audio from dsp
            recv_audio(dsp_connection_socket, &buf);

            // reverse audio
            reverse(&buf);
              
            // send reversed audio to dsp
            send_message(dsp_connection_socket, "done");
            recv_message(dsp_connection_socket, &receiver);
            if(receiver.compare("ready") == 0)
            {
                std::cout << "sending audio..." << std::endl;
                send_audio(dsp_connection_socket, &buf);
            }
            
            // close connection
            dsp_connection_socket->shutdown(asio::ip::tcp::socket::shutdown_both);
            dsp_connection_socket->close();

        }
        else if(receiver.compare("exit") == 0)
        {
            std::cout << "Exit" << std::endl;
            dsp_connection_socket->shutdown(asio::ip::tcp::socket::shutdown_both);
            dsp_connection_socket->close();
            return EXIT_SUCCESS;
        }
    }
    */
}