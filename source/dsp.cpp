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

    // create a new pipe using port 5500
    std::unique_ptr<DownstreamPipe> gui_service_pipe = std::make_unique<DownstreamPipe>(5500);
    gui_service_pipe->connect(); 

    // create a new pipe using port 8800
    std::unique_ptr<UpstreamPipe> reverse_service_pipe = 
        std::make_unique<UpstreamPipe>(8800, gui_service_pipe->get_context());

    while(1)
    {
        std::string receiver;
        receiver = gui_service_pipe->recv_message();
        
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
            
            // send ready message to gui
            gui_service_pipe->send_message("ready");
            
            // get audio from gui service
            std::shared_ptr<AudioFile<float>::AudioBuffer> audio = gui_service_pipe->recv_audio();
            std::cout << "received " << (*audio)[0].size() << " samples per channel " << std::endl;
            
            // connect to reverse microservice
            //system("./reverse.exe");
            reverse_service_pipe->connect();
            
            // send run command to reverse microservice
            reverse_service_pipe->send_message("run");
            
            // upon response, send audio
            receiver = reverse_service_pipe->recv_message();
            if(receiver.compare("ready") == 0)
            {
                reverse_service_pipe->send_audio(audio.get());
    
            }
            
            receiver = reverse_service_pipe->recv_message();        
            reverse_service_pipe->send_message("ready");

            audio = reverse_service_pipe->recv_audio();
            std::cout << "received " << (*audio)[0].size() << " samples per channel from reverse service" << std::endl;
            reverse_service_pipe->disconnect();
            
            // send reverse audio back to gui
            gui_service_pipe->send_audio(audio.get());
        }
        else if(receiver.compare(0, 4, "exit") == 0)
        {
            std::cout << "Exiting..." << std::endl;
            reverse_service_pipe->connect();
            reverse_service_pipe->send_message("exit");
            
            // close connection
            gui_service_pipe->disconnect();

            return EXIT_SUCCESS;
        }
    }
    return EXIT_SUCCESS;
}