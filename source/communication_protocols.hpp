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
    std::unique_ptr<asio::ip::tcp::endpoint> endpoint;
    std::unique_ptr<asio::io_context> context;
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
    endpoint.reset(new asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
    context.reset(new asio::io_context());
    socket.reset(new asio::ip::tcp::socket(*context));
}

//=================================================================================================
// Upstream Pipe Derived Class
//=================================================================================================
class UpstreamPipe : public Pipe
{
public:
    // constructor
    UpstreamPipe(): Pipe{}{};
    UpstreamPipe(int provided_port): Pipe{provided_port}
    {
        resolver.reset(new asio::ip::tcp::resolver(*context));
    }
    
    // manage connection
    bool connect();
    bool disconnect();

protected:
    std::unique_ptr<asio::ip::tcp::resolver> resolver;
};

// client requests connection to server
bool UpstreamPipe::connect()
{
    const std::string port_as_string = std::to_string(port);
    asio::connect(*socket, (*resolver).resolve("localhost", "5500"));

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
        acceptor.reset(new asio::ip::tcp::acceptor(*context, *endpoint));
    }

    // manage connection
    bool connect();
    bool disconnect();

protected:
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

/*=================================================================================================
// Send/Recv Audio Data
//=================================================================================================
const size_t Pipe::send_audio(AudioFile<float>::AudioBuffer* audio){

    // get the number of channels in the audio buffer
    const size_t num_channels = (*audio).size();
    if(num_channels < 1) return 0;
    std::cout << "num channels: " << num_channels << std::endl;

    // get the number of samples per channel in the audio buffer
    size_t samples_per_channel = (*audio)[0].size();

    // if the number of samples is equal to chunk size, pad audio data with 0's
    // this is a bandaid for bad end-write condition checking
    if(samples_per_channel % (chunk_size / sizeof(float)) == 0)
    {
        samples_per_channel++;
        for(size_t channel=0; channel<num_channels; channel++)
        {
            (*audio)[channel].resize(samples_per_channel);
            (*audio)[channel][samples_per_channel-1] = 0;
        }
    }
    std::cout << "samples per channel: " << samples_per_channel << std::endl;
    
    // wait for confirmation before sending channel's samples
    send_message("ready?");
    if(recv_message().compare("ready!") != 0){
        std::cout << "handshake refused" << std::endl;
        return false;
    }
    std::cout << "handshake accepted" << std::endl;
    
    // resize audio_chunk
    std::vector<float> audio_chunk;
    const size_t floats_per_chunk = chunk_size / sizeof(float);

    size_t total_write_size = 0;
    for(size_t channel=0; channel<2; channel++)
    {
        // write samples
        while(1)
        {
            unsigned int audio_sample = 0;
            audio_chunk.resize(floats_per_chunk);
            // break input into chunks
            //std::cout << "chunking samples..." << std::endl;
            for(size_t sample=0; sample<floats_per_chunk; sample++)
            {
                audio_chunk[sample] = (*audio)[channel][audio_sample++];
                if(audio_sample >= samples_per_channel)
                {
                    audio_chunk.resize(audio_sample % floats_per_chunk);
                    break;
                }
            }
            size_t write_size = audio_chunk.size() * sizeof(float);
            //std::cout << "samples chunked!" << std::endl;
            //std::cout << "size to write: " << write_size << std::endl;
            
            // wait for socket to become available for writing
            //std::cout << "waiting to write..." << std::endl;
            (*socket).wait(asio::socket_base::wait_type::wait_write);
            //std::cout << "ready to write..." << std::endl;
            
            // write chunked audio data to the socket
            write_size = asio::write(*socket, asio::buffer(audio_chunk, audio_chunk.size()*sizeof(float)));
            //std::cout << "write size: " << write_size << std::endl;
            
            total_write_size += write_size;
            //std::cout << "total write so far: " << total_write_size << std::endl;
            if(write_size < chunk_size || total_write_size >= (samples_per_channel * sizeof(float)))
            {
                std::cout << "breaking channel " << channel << std::endl;
                break;
            }
        }
        if(recv_message().compare("CONFIRMED") != 0) return total_write_size;
    }
    return total_write_size;
}

std::shared_ptr<AudioFile<float>::AudioBuffer> Pipe::recv_audio(){
    
    // create a new audio buffer
    std::shared_ptr<AudioFile<float>::AudioBuffer> buf = 
        std::make_shared<AudioFile<float>::AudioBuffer>();
    buf->resize(buf->size() + 1);

    std::cout << "configuring receiver" << std::endl;
    // configure receiver
    const size_t floats_per_chunk = chunk_size / sizeof(float);
    std::vector<float> receiver;
    receiver.resize(floats_per_chunk);
    std::cout << "receiver configured! size: " << receiver.size() << std::endl;

    // handshake with sending end
    if(recv_message().compare("ready?") != 0)
    {
        std::cout << "handshake refused" << std::endl;
        send_message("handshake refused");
        return NULL;
    }
    size_t write_size = send_message("ready!");
    std::cout << "handshake accepted" << std::endl;

    // start at beginning of channel data
    unsigned int buffer_idx = 0;
    size_t read_size = chunk_size;
    std::cout << "expected read size" << read_size << std::endl;
    for(size_t channel=0; channel<2; channel++)
    {
        buf->resize(channel + 1);
        while(1)
        {
            // wait for socket to become available for reading
            std::cout << "waiting to read..." << std::endl;
            (*socket).wait(asio::socket_base::wait_type::wait_read);
            std::cout << "ready to read" << std::endl;

            // read from the socket
            read_size = (*socket).read_some(asio::buffer(receiver, chunk_size));
            std::cout << "read size: " << read_size << std::endl;

            // add newly read floats to container
            size_t num_floats_read = read_size / sizeof(float);
            (*buf)[channel].resize((*buf)[channel].size() + num_floats_read);
            for(unsigned int sample=0; sample<num_floats_read; sample++)
            {
                (*buf)[channel][buffer_idx++] = receiver[sample];
            }
            
            // undersized chunk signals end of audio recv
            if(read_size < chunk_size)
            {
                std::cout << "breaking" << std::endl;
                break;
            }
        }
        // send confirmation
        std::cout << "sending confirmation" << std::endl;
        send_message("CONFIRMED");
    }
    return buf;
}
*/
/*================================================================================================
// Send Audio Data
//=================================================================================================
bool send_audio(std::shared_ptr<asio::ip::tcp::socket> socket,
                AudioFile<float>::AudioBuffer* samples){


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
    std::string msg;
    recv_message();
    if(msg.compare("CONFIRMED") != 0){
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
void recv_audio(std::shared_ptr<asio::ip::tcp::socket> socket,
                AudioFile<float>::AudioBuffer* container){
    
    (*container).clear();
    (*container).resize(2);

    std::vector<float> recv;
    
    // get left channel data
    recv.resize(CHUNK_SIZE);
    unsigned int buffer_idx = 0;
    while(1){
        size_t read_size = CHUNK_SIZE * sizeof(float);
        read_size = (*socket).read_some(asio::buffer(recv, read_size));
        //std::cout << "read size: " << read_size << std::endl;
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
    
    // send confirmation
    asio::error_code ignored_error;
    std::string msg = "CONFIRMED";
    send_message(socket, msg);

    // get right channel data
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
*/
#endif