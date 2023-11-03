#===============================================================================
#
#===============================================================================
cmake_minimum_required (VERSION 3.12)

project ("WaveTech"  VERSION 1.0.0
                     DESCRIPTION "A simple .WAV file editor."
                     HOMEPAGE_URL "https://github.com/grant-tm/WaveTech-Audio-Editor"
                     LANGUAGES CXX)

#===============================================================================
# NAME EXECUTABLE AND FORCE C++17 STANDARD
#===============================================================================
add_executable(Test test.cpp)
set_target_properties(Test PROPERTIES CXX_STANDARD 17)

#===============================================================================
# CONFIGURE DEPENDENCIES
#===============================================================================
add_library(AudioFile "../Wavetech-Audio-Editor/Dependencies/AudioFile.h")
set_target_properties(AudioFile PROPERTIES LINKER_LANGUAGE CXX)

#===============================================================================
# LINK DEPENDENCIES
#===============================================================================
target_link_libraries(Test INTERFACE AudioFile)