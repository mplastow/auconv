// files.hpp - auconv

#ifndef AUCONV_FILES_HPP
#define AUCONV_FILES_HPP

#include <iostream>
#include <string>

#include <sndfile.h>
#include <sndfile.hh>

namespace auconv {

void printFileInfo(std::string const& inputFile)
{
    // Print file properties with libsndfile
    SndfileHandle inputFileHandle = SndfileHandle(inputFile);
    if (inputFileHandle.error() != 0) {
        std::cout << "\tCould not open input file: " << inputFileHandle.strError() << '\n';
        return;
    } else {
        std::cout << "\tInput file: " << inputFile << '\n';
        std::cout << "\t\tFormat: 0x" << std::hex << inputFileHandle.format() << '\n';
        std::cout << "\t\tSample rate: " << std::dec << inputFileHandle.samplerate() << '\n';
        std::cout << "\t\tChannels: " << inputFileHandle.channels() << '\n';
        std::cout << "\t\tFrames in file: " << inputFileHandle.frames() << '\n';
    }
}

} // namespace auconv

#endif // AUCONV_FILES_HPP