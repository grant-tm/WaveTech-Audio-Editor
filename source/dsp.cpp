//=================================================================================================
// Include Dependencies
//=================================================================================================
#include "..\dependencies\AudioFile\AudioFile.h"

// objects for sending/receiving messages and audio data
#include "Pipe.hpp"

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

    while(1)
    {
        // create a new pipe using port 5500
        std::unique_ptr<DownstreamPipe> gui_service_pipe = std::make_unique<DownstreamPipe>(5500);
        std::cout << "waiting for connection from gui service" << std::endl;
        gui_service_pipe->connect(); 
        std::cout << "connected to gui service" << std::endl;
        
        std::cout << "waiting for message from gui service" << std::endl;
        std::string receiver;
        receiver = gui_service_pipe->recv_message();
        std::cout << "message received from gui service: " << receiver << std::endl;
        
        if(receiver.compare("shift") == 0)
        {
            std::cout << "shift" << std::endl;
        }
        else if(receiver.compare("stretch") == 0)
        {
            std::cout << "stretch"<< std::endl;
        }
        else if(receiver.compare("reverse") == 0)
        {           
            std::cout << "disconnecting from gui service" << std::endl;
            gui_service_pipe->disconnect();
            std::cout << "disconnected from gui service" << std::endl;

            // create a new pipe using port 8800
            std::unique_ptr<UpstreamPipe> reverse_service_pipe = 
                std::make_unique<UpstreamPipe>(8800, gui_service_pipe->get_context());
           
            std::cout << "connecting to reverse service" << std::endl;
            reverse_service_pipe->connect();
            std::cout << "connected to reverse service" << std::endl;
            
            reverse_service_pipe->send_message("run");
            
            std::cout << "waiting for response from reverse service" << std::endl;
            receiver = reverse_service_pipe->recv_message();
            if(receiver.compare("ready") == 0)
            {
                std::cout << "reverse service confirmed responsive" << std::endl;
                //reverse_service_pipe->send_audio(audio.get());
            }
            //gui_service_pipe->send_audio(audio.get());
            reverse_service_pipe->disconnect();
        }
        else if(receiver.compare(0, 4, "exit") == 0)
        {
            std::cout << "Exiting..." << std::endl;
            
            // create a new pipe using port 8800
            std::unique_ptr<UpstreamPipe> reverse_service_pipe = 
                std::make_unique<UpstreamPipe>(8800, gui_service_pipe->get_context());
           
            std::cout << "connecting to reverse service" << std::endl;
            reverse_service_pipe->connect();
            std::cout << "connected to gui service" << std::endl;

            //send run command and audio over port 8800
            reverse_service_pipe->send_message("exit");
            
            // close connection
            gui_service_pipe->disconnect();

            return EXIT_SUCCESS;
        }
    }
    return EXIT_SUCCESS;
}