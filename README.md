# WaveTech-Audio-Editor
An audio editor for .wav files.

## Features
Load a local .wav file and perform the following operations:
1. Pitch Shift +-12 semitones
2. Time Stretch 50-200%
3. Reverse Audio

## Dependencies
### [AudioFile](https://github.com/adamstark/AudioFile) by [AdamStark](https://github.com/adamstark)
The AudioFile library is used to extract, edit, and save raw audio data to/from a .wav file.
### [libzmq](https://github.com/zeromq/libzmq) by [ZeroMQ](https://github.com/zeromq)
ZeroMQ is an asynchronous networking library, which is required for communication between WaveTech's GUI and signal processor. 
The libzmq library is a C implementation powering (most of) ZeroMQ's language-specific bindings.
### [cppzmq](https://github.com/zeromq/cppzmq) by [ZeroMQ](https://github.com/zeromq)
The cppzmq library is a lightweight C++ binding of ZeroMQ.

## Build Instructions
1. Clone this repository.
2. Navigate to the `WaveTech-Audio-Editor/build` directory.
3. Run `cmake -B. -S..`
