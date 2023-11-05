#include "..\dependencies\AudioFile\AudioFile.h"
#include <iostream>
#include <stdlib.h>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include "..\dependencies\asio\include\asio.hpp"

int main(int argc, char** argv){
    return 1;
}