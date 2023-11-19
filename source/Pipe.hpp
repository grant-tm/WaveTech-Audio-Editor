#ifndef _PIPE
#define _PIPE

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

//=================================================================================================
// Standard Library
//=================================================================================================
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <memory>

//=================================================================================================
// Global Variables
//=================================================================================================
//number of floats to write, so (CHUNK_SIZE * sizeof(float) is the number of bytes to write)
#define CHUNK_SIZE 1024

//=================================================================================================
// Pipe Parent Class
//=================================================================================================
class Pipe
{
public:
    // constructor
    Pipe();
    Pipe(int);
    Pipe(int, std::shared_ptr<asio::io_context>);

    // retrieve the io context
    std::shared_ptr<asio::io_context> get_context();

    // manage connection
    virtual bool connect()=0; 
    virtual bool disconnect()=0;
    
    // send/recv string messages
    const size_t send_message(const std::string);
    const std::string recv_message();

    // send/recv float audio data
    bool send_audio(AudioFile<float>::AudioBuffer*);
    std::shared_ptr<AudioFile<float>::AudioBuffer> recv_audio();

protected:
    int port;
    size_t chunk_size;
    bool connected;
    std::shared_ptr<asio::io_context> context;
    std::shared_ptr<asio::ip::tcp::socket> socket;
    std::unique_ptr<asio::error_code> error;
};

Pipe::Pipe()
{
    chunk_size = 1024;
    connected = false;
}

Pipe::Pipe(int provided_port)
{
    port = provided_port;
    chunk_size = 1024;
    connected = false;
    context.reset(new asio::io_context());
    socket.reset(new asio::ip::tcp::socket(*context));
}

Pipe::Pipe(int provided_port, std::shared_ptr<asio::io_context> provided_context)
{
    port = provided_port;
    chunk_size = 1024;
    connected = false;
    context = provided_context;
    socket.reset(new asio::ip::tcp::socket(*context));
}

std::shared_ptr<asio::io_context> Pipe::get_context()
{
    return context;
}

//=================================================================================================
// Upstream Pipe Derived Class
//=================================================================================================
class UpstreamPipe : public Pipe
{
public:
    // constructor
    UpstreamPipe(): Pipe{}{};
    UpstreamPipe(int provided_port): 
    Pipe{provided_port}
    {
        resolver.reset(new asio::ip::tcp::resolver(*context));
        endpoint = resolver->resolve("localhost", std::to_string(provided_port));
    }
    UpstreamPipe(int provided_port, std::shared_ptr<asio::io_context> provided_context): 
    Pipe{provided_port, provided_context}
    {
        resolver.reset(new asio::ip::tcp::resolver(*context));
        endpoint = resolver->resolve("localhost", std::to_string(provided_port));
    }
    
    // manage connection
    bool connect();
    bool disconnect();

protected:
    std::unique_ptr<asio::ip::tcp::resolver> resolver;
    asio::ip::tcp::resolver::results_type endpoint;
};

// client requests connection to server
bool UpstreamPipe::connect()
{
    asio::connect(*socket, endpoint);
    connected = true;
    return connected;
}

// server handles disconnect. just update status
bool UpstreamPipe::disconnect()
{
    connected = false;
    return connected;
}

//=================================================================================================
// Downstream Pipe Derived Class
//=================================================================================================
class DownstreamPipe : public Pipe
{
public:
    // constructor
    DownstreamPipe(): Pipe{}{}
    DownstreamPipe(int provided_port): Pipe{provided_port}
    {
        endpoint.reset(new asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
        acceptor.reset(new asio::ip::tcp::acceptor(*context, *endpoint));
    }
    DownstreamPipe(int provided_port, std::shared_ptr<asio::io_context> provided_context): 
    Pipe{provided_port, provided_context}
    {
        endpoint.reset(new asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
        acceptor.reset(new asio::ip::tcp::acceptor(*context, *endpoint));
    }

    // manage connection
    bool connect();
    bool disconnect();

protected:
    std::unique_ptr<asio::ip::tcp::endpoint> endpoint;
    std::shared_ptr<asio::ip::tcp::acceptor> acceptor;
};

// server accepts connection from client
bool DownstreamPipe::connect()
{
    (*acceptor).accept(*socket);
    
    connected = true;
    return true;
}

// server disconnect protocol
bool DownstreamPipe::disconnect()
{
    socket->shutdown(asio::ip::tcp::socket::shutdown_both);
    socket->close();
    
    connected = false;
    return connected;
}

//=================================================================================================
// Send/Recv Messages
//=================================================================================================
const size_t Pipe::send_message(const std::string message)
{
    (*socket).wait(asio::socket_base::wait_type::wait_write);
    const size_t write_size = asio::write(*socket, asio::buffer(message));
    return write_size;
}

const std::string Pipe::recv_message()
{
    // configure receiver
    std::string receiver;
    receiver.resize(Pipe::chunk_size);
    
    // receive message
    (*socket).wait(asio::socket_base::wait_type::wait_read);
    const size_t read_size = (*socket).read_some(asio::buffer(receiver));
    receiver.resize(read_size);
    
    // return message
    const std::string message = receiver;
    return message;
}

//================================================================================================
// Send Audio Data
//=================================================================================================
bool Pipe::send_audio(AudioFile<float>::AudioBuffer* samples)
{
    asio::error_code ignored_error;
    std::vector<float> chunked_samples;

    // send left channel data
    chunked_samples.resize(CHUNK_SIZE);
    unsigned int buffer_idx = 0;
    size_t buffer_size = (*samples)[0].size();
    bool write_done = false;
    while(1){
        // break samples into chunks
        for(unsigned int i=0; i<CHUNK_SIZE; i++){
            chunked_samples[i] = (*samples)[0][buffer_idx++];
            if(buffer_idx >= buffer_size){
                chunked_samples.resize(buffer_idx % CHUNK_SIZE);
                write_done = true;
                break;
            }
        }
        // write chunks to socket
        size_t write_size = chunked_samples.size() * sizeof(float);
        write_size = asio::write(*socket, asio::buffer(chunked_samples, write_size), ignored_error);
        
        // break when less than a full chunk is used
        unsigned int num_samples_written = static_cast<unsigned int>(write_size/sizeof(float));
        if(write_done){
            break;
        }
    }
    
    // wait for confirmation
    if(recv_message().compare("CONFIRMED") != 0){
        return false;
    }

    // send right channel data
    chunked_samples.resize(CHUNK_SIZE);
    buffer_idx = 0;
    buffer_size = (*samples)[1].size();
    write_done = false;
    while(1){
        // break samples into chunks
        for(unsigned int i=0; i<CHUNK_SIZE; i++){
            chunked_samples[i] = (*samples)[1][buffer_idx++];
            if(buffer_idx >= buffer_size){
                chunked_samples.resize(buffer_idx % CHUNK_SIZE);
                write_done = true;
                break;
            }
        }
        // write chunks to socket
        size_t write_size = chunked_samples.size() * sizeof(float);
        write_size = asio::write(*socket, asio::buffer(chunked_samples, write_size), ignored_error);
        // break when less than a full chunk is used
        unsigned int num_samples_written = static_cast<unsigned int>(write_size/sizeof(float));
        if(write_done){
            break;
        }
    }

    return true;
}
//=================================================================================================
// Receive Audio Data
//=================================================================================================
std::shared_ptr<AudioFile<float>::AudioBuffer> Pipe::recv_audio()
{
    std::shared_ptr<AudioFile<float>::AudioBuffer> container = 
        std::make_shared<AudioFile<float>::AudioBuffer>();

    (*container).clear();
    (*container).resize(2);

    std::vector<float> recv;
    
    // get left channel data
    recv.resize(CHUNK_SIZE);
    unsigned int buffer_idx = 0;
    while(1)
    {
        size_t read_size = CHUNK_SIZE * sizeof(float);
        read_size = (*socket).read_some(asio::buffer(recv, read_size));
        //std::cout << "read size: " << read_size << std::endl;
        unsigned int num_floats_read = static_cast<unsigned int>(read_size / sizeof(float));
        if((*container)[0].size() < ((*container)[0].size() + num_floats_read))
        {
            (*container)[0].resize((*container)[0].size() + num_floats_read);
        }
        for(unsigned int i=0; i<num_floats_read; i++)
        {
            (*container)[0][buffer_idx++] = recv[i];
        }
        if(num_floats_read < CHUNK_SIZE)
        {
            break;
        }
    }
    
    // send confirmation
    send_message("CONFIRMED");

    // get right channel data
    recv.resize(CHUNK_SIZE);
    buffer_idx = 0;
    while(1)
    {
        size_t read_size = CHUNK_SIZE * sizeof(float);
        read_size = (*socket).read_some(asio::buffer(recv, read_size));
        unsigned int num_floats_read = static_cast<unsigned int>(read_size / sizeof(float));
        if((*container)[1].size() < ((*container)[1].size() + num_floats_read))
        {
            (*container)[1].resize((*container)[1].size() + num_floats_read);
        }
        for(unsigned int i=0; i<num_floats_read; i++)
        {
            (*container)[1][buffer_idx++] = recv[i];
        }
        if(num_floats_read < CHUNK_SIZE)
        {
            break;
        }
    }
    return container;
}

#endif