// snd.cpp - auconv

#include <snd.hpp>

#include <filesystem>
#include <iostream>

namespace auconv {

void convertAudioFile(std::filesystem::path const& inputFile, std::filesystem::path const& outputFile, int outputFormat)
{
    // Get handle to input file
    SndfileHandle inputFileHandle = SndfileHandle(inputFile);
    if (inputFileHandle.error()) {
        std::cout << "Could not open input file: " << inputFileHandle.strError() << '\n';
        return;
    } else {
        printFileInfo(inputFile);
    }

    // Get handle to output file
    SndfileHandle outputFileHandle = SndfileHandle(
        outputFile,
        SFM_WRITE,
        outputFormat,
        inputFileHandle.channels(),
        inputFileHandle.samplerate());
    // Exit if an error occurred
    if (outputFileHandle.error()) {
        std::cout << "\tCould not open output file: " << outputFileHandle.strError() << '\n';
        std::cout << "\tCheck against specified format (1 = pass): " << outputFileHandle.formatCheck(outputFormat, inputFileHandle.channels(), inputFileHandle.samplerate()) << '\n';
        return;
    } else { // similar to PrintFileInfo()
        std::cout << "\tOutput file: " << outputFile << '\n';
        std::cout << "\t\tFormat: 0x" << std::hex << outputFileHandle.format() << '\n';
        std::cout << "\t\tSample rate: " << std::dec << outputFileHandle.samplerate() << '\n';
        std::cout << "\t\tChannels: " << outputFileHandle.channels() << '\n';
    }

    // Initialize conversion buffer
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

} // namespace auconv