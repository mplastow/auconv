// flac.cpp - auconv

#include <flac.hpp>

#include <array>
#include <filesystem>
#include <iostream>

#include <sndfile.hh>

#include <defines.hpp>
#include <files.hpp>
#include <types.hpp>

namespace auconv {

constexpr auto FORMAT_FLAC_PCM_16 = SF_FORMAT_FLAC | SF_FORMAT_PCM_16;

namespace {

    void convertDirEntryToFlac(std::filesystem::directory_entry const& dir_entry)
    {
        if (dir_entry.is_regular_file()) {
            if (dir_entry.path().extension() == ".wav") {
                std::cout << "Converting wav file: " << dir_entry.path().filename() << '\n';

                std::string outputFile = dir_entry.path().parent_path();
                outputFile.append("/").append(dir_entry.path().stem()).append(".flac");

                convertWavToFlacFile(dir_entry.path(), outputFile, FORMAT_FLAC_PCM_16);
            } else {
                std::cout << "Skipping file: " << dir_entry.path().filename() << '\n';
            }
        } else {
            std::cout << "Directory: " << dir_entry << '\n';
        }
    }

    void doBufferReadWrite(SndfileHandle& in, SndfileHandle& out)
    { // Initialize conversion buffer
        std::array<short, BUFFER_SIZE> buffer {};

        // Read through file to convert from input format to output format
        // and write to output file
        while (true) {
            auto read = in.read(buffer.data(), buffer.size());

            // No more bytes to be read, so we're done
            if (read == 0) {
                return;
            }

            // Minimal buffer safety check
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
        printOutputFileError(in_handle, out_handle, format);
        return;
    } else {
        printFileInfo(out_handle, output);
    }

    doBufferReadWrite(in_handle, out_handle);

    std::cout << "\tFinished file conversion." << '\n';
}

void convertWavToFlacInDir(Path const& path)
{
    for (auto const& dir_entry : std::filesystem::directory_iterator(path)) {
        convertDirEntryToFlac(dir_entry);
    }
}

void convertWavToFlacInDirTree(Path const& path)
{
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(path)) {
        convertDirEntryToFlac(dir_entry);
    }
}

} // namespace auconv