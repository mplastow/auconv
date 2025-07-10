// files.cpp - auconv

#include <files.hpp>

#include <sndfile.hh>

#include <iostream>

namespace auconv {

void printFileInfo(std::filesystem::path const& inputFile)
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