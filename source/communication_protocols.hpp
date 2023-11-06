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

//number of floats to write, so (CHUNK_SIZE * sizeof(float) is the number of bytes to write)
#define CHUNK_SIZE 1024

/*=====================================================================================================================
== Send Audio Data
=====================================================================================================================*/
bool send_audio(std::shared_ptr<asio::ip::tcp::socket> socket,
                AudioFile<float>::AudioBuffer* samples){


    asio::error_code ignored_error;
    std::vector<float> chunked_samples;
    
    /* send left channel data */
    chunked_samples.resize(CHUNK_SIZE);
    unsigned int buffer_idx = 0;
    size_t buffer_size = (*samples)[0].size();
    while(1){
        /* break samples into chunks */
        for(unsigned int i=0; i<CHUNK_SIZE; i++){
            chunked_samples[i] = (*samples)[0][buffer_idx++];
            if(buffer_idx >= buffer_size){
                chunked_samples.resize(buffer_idx % CHUNK_SIZE);
                break;
            }
        }
        /* write chunks to socket */
        size_t write_size = chunked_samples.size() * sizeof(float);
        write_size = asio::write(*socket, asio::buffer(chunked_samples, write_size), ignored_error);
        
        /* break when less than a full chunk is used */
        unsigned int num_samples_written = static_cast<unsigned int>(write_size/sizeof(float));
        if(num_samples_written < CHUNK_SIZE){
            break;
        }
    }
    
    /* wait for confirmation */
    std::string msg = "transfer confirmed";
    while(1){
        size_t read_size = (*socket).read_some(asio::buffer(msg));
        if(msg == "TRANSFER CONFIRMED"){
            break;
        }
    }

    /* send left channel data */
    chunked_samples.resize(CHUNK_SIZE);
    buffer_idx = 0;
    buffer_size = (*samples)[1].size();
    while(1){
        /* break samples into chunks */
        for(unsigned int i=0; i<CHUNK_SIZE; i++){
            chunked_samples[i] = (*samples)[1][buffer_idx++];
            if(buffer_idx >= buffer_size){
                chunked_samples.resize(buffer_idx % CHUNK_SIZE);
                break;
            }
        }
        /* write chunks to socket */
        size_t write_size = chunked_samples.size() * sizeof(float);
        write_size = asio::write(*socket, asio::buffer(chunked_samples, write_size), ignored_error);
        /* break when less than a full chunk is used */
        unsigned int num_samples_written = static_cast<unsigned int>(write_size/sizeof(float));
        if(num_samples_written < CHUNK_SIZE){
            break;
        }
    }
    
    return true;
}
/*=====================================================================================================================
== Receive Audio Data
=====================================================================================================================*/
void recv_audio(std::shared_ptr<asio::ip::tcp::socket> socket,
                AudioFile<float>::AudioBuffer* container){
    
    std::vector<float> recv;
    
    /* get left channel data */
    recv.resize(CHUNK_SIZE);
    unsigned int buffer_idx = 0;
    while(1){
        size_t read_size = CHUNK_SIZE * sizeof(float);
        read_size = (*socket).read_some(asio::buffer(recv, read_size));
        unsigned int num_floats_read = static_cast<unsigned int>(read_size / sizeof(float));
        if((*container)[0].size() < ((*container)[0].size() + num_floats_read)){
            (*container)[0].resize((*container)[0].size() + num_floats_read);
        }
        for(unsigned int i=0; i<num_floats_read; i++){
            (*container)[0][buffer_idx++] = recv[i];
        }
        if(num_floats_read < CHUNK_SIZE){
            break;
        }
    }
    
    /* send confirmation */
    asio::error_code ignored_error;
    std::string msg = "TRANSFER CONFIRMED";
    size_t write_size = asio::write(*socket, asio::buffer(msg), ignored_error);

    /* get right channel data */
    recv.resize(CHUNK_SIZE);
    buffer_idx = 0;
    while(1){
        size_t read_size = CHUNK_SIZE * sizeof(float);
        read_size = (*socket).read_some(asio::buffer(recv, read_size));
        unsigned int num_floats_read = static_cast<unsigned int>(read_size / sizeof(float));
        if((*container)[1].size() < ((*container)[1].size() + num_floats_read)){
            (*container)[1].resize((*container)[1].size() + num_floats_read);
        }
        for(unsigned int i=0; i<num_floats_read; i++){
            (*container)[1][buffer_idx++] = recv[i];
        }
        if(num_floats_read < CHUNK_SIZE){
            break;
        }
    }
}

#endif