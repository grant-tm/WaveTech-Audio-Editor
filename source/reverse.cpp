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
void reverse(AudioFile<float>::AudioBuffer* buffer)
{
    size_t num_channels = (*buffer).size();
    for(size_t channel=0; channel<num_channels; channel++)
    {
       std::reverse((*buffer)[channel].begin(), (*buffer)[channel].end());
    }
}
/*=====================================================================================================================
== Main
=====================================================================================================================*/
int main(int argc, char** argv)
{

    while(1)
    {   
        // create a new pipe using port 8800
        std::unique_ptr<DownstreamPipe> pipe = std::make_unique<DownstreamPipe>(8800);
        // wait for connection
        std::cout << "waiting for connection" << std::endl;
        std::cout << pipe->connect() << std::endl;
        std::cout << "connected" << std::endl;
        
        std::cout << "waiting for message" << std::endl;
        std::string receiver = pipe->recv_message();
        std::cout << "message received: " << receiver << std::endl;

        if(receiver.compare("run") == 0)
        {
            // handshake with dsp
            pipe->send_message("ready");
            /*
            // receive audio from dsp
            std::shared_ptr<AudioFile<float>::AudioBuffer> buf = pipe->recv_audio();

            // reverse audio
            reverse(buf.get());
              
            // send reversed audio to dsp
            pipe->send_message("done");
            receiver = pipe->recv_message();
            if(receiver.compare("ready") == 0)
            {
                pipe->send_audio(buf.get());
            }
            */
            // close connection
            pipe->disconnect();
        }
        else if(receiver.compare("exit") == 0)
        {
            std::cout << "Exit" << std::endl;
            pipe->disconnect();
            return EXIT_SUCCESS;
        }
    }
}