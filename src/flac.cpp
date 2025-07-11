// flac.cpp - auconv

#include <flac.hpp>

#include <filesystem>
#include <iostream>

#include <sndfile.hh>

#include <defines.hpp>
#include <files.hpp>

namespace auconv {

using Path = std::filesystem::path;

void convertWavToFlacFile(Path const& input, Path const& output, int format)
{
    // Get handle to input file
    SndfileHandle inputFileHandle = SndfileHandle(input);
    if (inputFileHandle.error()) {
        std::cout << "Could not open input file: " << inputFileHandle.strError() << '\n';
        return;
    } else {
        printFileInfo(input);
    }

    // Get handle to output file
    SndfileHandle outputFileHandle = SndfileHandle(
        output,
        SFM_WRITE,
        format,
        inputFileHandle.channels(),
        inputFileHandle.samplerate());
    // Exit if an error occurred
    if (outputFileHandle.error()) {
        std::cout << "\tCould not open output file: " << outputFileHandle.strError() << '\n';
        std::cout << "\tCheck against specified format (1 = pass): " << outputFileHandle.formatCheck(format, inputFileHandle.channels(), inputFileHandle.samplerate()) << '\n';
        return;
    } else { // similar to PrintFileInfo()
        std::cout << "\tOutput file: " << output << '\n';
        std::cout << "\t\tFormat: 0x" << std::hex << outputFileHandle.format() << '\n';
        std::cout << "\t\tSample rate: " << std::dec << outputFileHandle.samplerate() << '\n';
        std::cout << "\t\tChannels: " << outputFileHandle.channels() << '\n';
    }

    // Initialize conversion buffer
    // TODO(MATT): use std::array instead
    static short buffer[BUFFER_SIZE];
    // std::cout << "Read/write buffer size: " << BUFFER_SIZE << std::endl;

    // Initialize read and write objects
    sf_count_t read = 1;
    sf_count_t write = 1;

    // Read through file to convert from input format to output format
    // and write to output file
    while (read != 0) {
        read = inputFileHandle.read(buffer, BUFFER_SIZE);
        // std::cout << "Read: " << read << std::endl;

        if (read != 0) { // write to out if bytes were read
            write = outputFileHandle.write(buffer, read);
            // std::cout << "Write: " << write << std::endl;
        }
    }

    std::cout << "\tFinished file conversion." << '\n';
    // std::cout << " Reached end of input file. Last read: " << read << std::endl;
    if (write == 0) {
        //     std::cout << "Warning: Last write was 0 bytes! Last write: " << write << "\n" << std::endl;
    } else {
        //     std::cout << "Last write: " << write << "\n" << std::endl;
    }
}

void convertWavToFlacInDir(Path const& path)
{
    for (auto const& dir_entry : std::filesystem::directory_iterator(path)) {
        if (dir_entry.is_regular_file()) {
            if (dir_entry.path().extension() == ".wav") {
                std::cout << "Converting wav file: " << dir_entry.path().filename() << '\n';

                std::string outputFile = dir_entry.path().parent_path();
                outputFile.append("/").append(dir_entry.path().stem()).append(".flac");

                convertWavToFlacFile(
                    dir_entry.path(),
                    outputFile,
                    SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
            }
        } else {
            std::cout << "Directory: " << dir_entry << '\n';
        }
    }
}

void convertWavToFlacInDirTree(Path const& path)
{
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(path)) {
        if (dir_entry.is_regular_file()) {
            if (dir_entry.path().extension() == ".wav") {
                std::cout << "Converting wav file: " << dir_entry.path().filename() << '\n';

                std::string outputFile = dir_entry.path().parent_path();
                outputFile.append("/").append(dir_entry.path().stem()).append(".flac");

                convertWavToFlacFile(
                    dir_entry.path(),
                    outputFile,
                    SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
            }
        } else {
            std::cout << "Directory: " << dir_entry << '\n';
        }
    }
}

} // namespace auconv