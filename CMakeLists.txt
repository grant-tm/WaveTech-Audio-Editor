#===============================================================================
# CONFIGURE CMAKE
#===============================================================================
cmake_minimum_required (VERSION 3.12)

#===============================================================================
# CONFIGURE PROJECT
#===============================================================================
project (WaveTech   
        VERSION 1.0.0
        DESCRIPTION "A simple .WAV file editor."
        HOMEPAGE_URL "https://github.com/grant-tm/WaveTech-Audio-Editor"
        LANGUAGES CXX)

#===============================================================================
# CONFIGURE EXECUTABLES
#===============================================================================
add_executable          (dsp "source/dsp.cpp")
set_target_properties   (dsp PROPERTIES CXX_STANDARD 17)

add_executable          (gui "source/gui.cpp")
set_target_properties   (gui PROPERTIES CXX_STANDARD 17)

#===============================================================================
# CONFIGURE DEPENDENCIES
#===============================================================================
add_library             (audiofile_lib "dependencies/AudioFile/AudioFile.h")
set_target_properties   (audiofile_lib PROPERTIES LINKER_LANGUAGE CXX)

add_library             (asio "dependencies/asio/include/asio.hpp")
set_target_properties   (asio PROPERTIES LINKER_LANGUAGE CXX)

#===============================================================================
# LINK DEPENDENCIES
#===============================================================================
target_link_libraries   (dsp INTERFACE audiofile_lib)
target_link_libraries   (dsp INTERFACE asio)

target_link_libraries   (gui INTERFACE audiofile_lib)
target_link_libraries   (gui INTERFACE asio)