// flac.cpp - auconv

#include <flac.hpp>

#include <array>
#include <filesystem>
#include <iostream>

#include <sndfile.hh>

#include <defines.hpp>
#include <files.hpp>

namespace auconv {

constexpr auto FORMAT_FLAC_PCM_16 = SF_FORMAT_FLAC | SF_FORMAT_PCM_16;

using Path = std::filesystem::path;

namespace {

    void doBufferReadWrite(SndfileHandle& in, SndfileHandle& out)
    { // Initialize conversion buffer
        std::array<short, BUFFER_SIZE> buffer {};

        // Read through file to convert from input format to output format
        // and write to output file
        sf_count_t read = 1;
        while (read != 0) {
            read = in.read(buffer.data(), buffer.size());

            // No more bytes to be read, so we're done
            if (read == 0) {
                return;
            }

            if (static_cast<long>(buffer.size()) < read) {
                std::cout << "Read too many bytes, bailing out!\n";
                std::quick_exit(1);
            }

            out.write(buffer.data(), read);
        }
    }
}

void convertWavToFlacFile(Path const& input, Path const& output, int format)
{
    // Get handle to input file
    SndfileHandle in_handle { input };
    if (in_handle.error() != 0) {
        std::cout << "Could not open input file: " << in_handle.strError() << '\n';
        return;
    } else {
        printFileInfo(input);
    }

    // Get handle to output file
    SndfileHandle out_handle {
        output,
        SFM_WRITE,
        format,
        in_handle.channels(),
        in_handle.samplerate()
    };

    // Exit if an error occurred
    if (out_handle.error() != 0) {
        std::cout << "\tCould not open output file: " << out_handle.strError() << '\n';
        std::cout << "\tCheck against specified format (1 = pass): "
                  << out_handle.formatCheck(
                         format,
                         in_handle.channels(),
                         in_handle.samplerate())
                  << '\n';
        return;
    } else { // similar to PrintFileInfo()
        std::cout << "\tOutput file: " << output << '\n';
        std::cout << "\t\tFormat: 0x" << std::hex << out_handle.format() << '\n';
        std::cout << "\t\tSample rate: " << std::dec << out_handle.samplerate() << '\n';
        std::cout << "\t\tChannels: " << out_handle.channels() << '\n';
    }

    doBufferReadWrite(in_handle, out_handle);

    std::cout << "\tFinished file conversion." << '\n';
}

void convertWavToFlacInDir(Path const& path)
{
    for (auto const& dir_entry : std::filesystem::directory_iterator(path)) {
        if (dir_entry.is_regular_file()) {
            if (dir_entry.path().extension() == ".wav") {
                std::cout << "Converting wav file: " << dir_entry.path().filename() << '\n';

                std::string outputFile = dir_entry.path().parent_path();
                outputFile.append("/").append(dir_entry.path().stem()).append(".flac");

                convertWavToFlacFile(dir_entry.path(), outputFile, FORMAT_FLAC_PCM_16);
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

                convertWavToFlacFile(dir_entry.path(), outputFile, FORMAT_FLAC_PCM_16);
            }
        } else {
            std::cout << "Directory: " << dir_entry << '\n';
        }
    }
}

} // namespace auconv