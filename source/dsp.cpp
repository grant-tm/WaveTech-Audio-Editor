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
// Connect to reverse microservice
//=================================================================================================
void reverse(asio::io_context& io_context, AudioFile<float>::AudioBuffer* buf)
{
    // connect to reverse microservice
    std::shared_ptr<asio::ip::tcp::socket> reverse_connection_socket(new asio::ip::tcp::socket(io_context));
    asio::ip::tcp::resolver resolver(io_context);
    asio::connect(*reverse_connection_socket, resolver.resolve("localhost", "8800"));
    
    // send run command and audio over port 8800
    send_message(reverse_connection_socket, "run");
    std::string receiver;
    recv_message(reverse_connection_socket, &receiver);
    if(receiver.compare("ready") == 0)
    {
        send_audio(reverse_connection_socket, buf);
    }
    recv_message(reverse_connection_socket, &receiver);
    
    // receive response from reverse
    send_message(reverse_connection_socket, "ready");
    recv_audio(reverse_connection_socket, buf);

    return;
}

//=================================================================================================
// Main
//=================================================================================================
int main(int argc, char** argv)
{
    // receive connection from gui
    asio::io_context io_context;
    std::shared_ptr<asio::ip::tcp::socket> gui_connection_socket(new asio::ip::tcp::socket(io_context));
    std::shared_ptr<asio::ip::tcp::acceptor> acceptor_ptr
        (new asio::ip::tcp::acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 5500)));
    (*acceptor_ptr).accept(*gui_connection_socket);

    while(1)
    {
        std::cout << "waiting..." << std::endl;
        
        std::string receiver;
        recv_message(gui_connection_socket, &receiver);
        std::cout << "Message received: " << receiver << std::endl;
        if(receiver.compare(0, 5, "shift") == 0)
        {
            std::cout << "shift" << std::endl;
        }
        else if(receiver.compare(0, 6, "stretch") == 0)
        {
            std::cout << "stretch"<< std::endl;
        }
        else if(receiver.compare(0, 7, "reverse") == 0)
        {
            //create new audio buffer
            AudioFile<float>::AudioBuffer buf;
            buf.resize(2);
            // receive audio from the gui
            recv_audio(gui_connection_socket, &buf);
            // send audio to reverse microservice
            reverse(io_context, &buf);
            // send reversed audio back to the gui
            send_audio(gui_connection_socket, &buf);
        }
        else if(receiver.compare(0, 4, "exit") == 0){
            std::cout << "Exiting..." << std::endl;
            
            // connect to reverse
            std::shared_ptr<asio::ip::tcp::socket> reverse_connection_socket(new asio::ip::tcp::socket(io_context));
            asio::ip::tcp::resolver resolver(io_context);
            asio::connect(*reverse_connection_socket, resolver.resolve("localhost", "8800"));
            
            //send run command and audio over port 8800
            send_message(reverse_connection_socket, "exit");
            
            // close connection
            gui_connection_socket->shutdown(asio::ip::tcp::socket::shutdown_both);
            gui_connection_socket->close();

            return EXIT_SUCCESS;
        }
    }
    return EXIT_SUCCESS;
}