// files.cpp - auconv

#include <files.hpp>

#include <sndfile.hh>

#include <iostream>

namespace auconv {

void printFileInfo(Path const& input_file)
{
    // Print file properties with libsndfile
    SndfileHandle input_file_handle = SndfileHandle(input_file);
    if (input_file_handle.error() != 0) {
        std::cout << "\tCould not open input file: " << input_file_handle.strError() << '\n';
        return;
    } else {
        std::cout << "\tFile: " << input_file << '\n';
        std::cout << "\t\tFormat: 0x" << std::hex << input_file_handle.format() << '\n';
        std::cout << "\t\tSample rate: " << std::dec << input_file_handle.samplerate() << '\n';
        std::cout << "\t\tChannels: " << input_file_handle.channels() << '\n';
        std::cout << "\t\tFrames in file: " << input_file_handle.frames() << '\n';
    }
}

void printFileInfo(SndfileHandle const& handle, Path const& path)
{
    // Print file properties with libsndfile
    if (handle.error() != 0) {
        std::cout << "\tCould not open input file: " << handle.strError() << '\n';
        return;
    } else {
        std::cout << "\tFile: " << path << '\n';
        std::cout << "\t\tFormat: 0x" << std::hex << handle.format() << '\n';
        std::cout << "\t\tSample rate: " << std::dec << handle.samplerate() << '\n';
        std::cout << "\t\tChannels: " << handle.channels() << '\n';
        std::cout << "\t\tFrames in file: " << handle.frames() << '\n';
    }
}

void printOutputFileError(SndfileHandle const& in_handle, SndfileHandle const& out_handle, int format)
{
    std::cout << "\tCould not open output file: " << out_handle.strError() << '\n';
    std::cout << "\tCheck against specified format (1 = pass): "
              << out_handle.formatCheck(
                     format,
                     in_handle.channels(),
                     in_handle.samplerate())
              << '\n';
}

} // namespace auconv