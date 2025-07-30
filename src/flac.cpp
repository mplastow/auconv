// flac.cpp - auconv

#include <cstdlib>
#include <flac.hpp>

#include <array>
#include <filesystem>
#include <iostream>

#include <sndfile.h>
#include <sndfile.hh>

#include <defines.hpp>
#include <files.hpp>
#include <snd.hpp>
#include <types.hpp>

namespace auconv {

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
        while (true) {
            // Read a chunk of data into the buffer
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

            // Write the chunk from buffer to file
            out.write(buffer.data(), read);
        }
    }

} // namespace

void convertWavToFlacFile(Path const& input, Path const& output, int format)
{
    // Get handle to input file
    SndfileHandle in_handle {input};
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
        in_handle.samplerate()};

    // Bail out if an error occurred
    if (out_handle.error() != 0) {
        printErrorOutputFile(in_handle, out_handle, format);
        return;
    }

    printFileInfo(out_handle, output);

    doBufferReadWrite(in_handle, out_handle);

    std::cout << "\tFinished file conversion." << '\n';
}

/// TODO: (MATT)  These two functions are good candidates for a very basic Strategy pattern
void convertWavToFlacInDirs(Path const& path, PathType path_type)
{
    using namespace std::filesystem;

    switch (path_type) {
    case PathType::Directory: {
        for (directory_entry const& dir_entry : directory_iterator(path)) {
            convertDirEntryToFlac(dir_entry);
        }
    } break;
    case PathType::DirectoryTree: {
        for (directory_entry const& dir_entry : recursive_directory_iterator(path)) {
            convertDirEntryToFlac(dir_entry);
        }
    } break;
    case PathType::File: {
        convertDirEntryToFlac(directory_entry(path));
    } break;
    default: {
        std::cout << "Bad file got here somehow\n";
        std::quick_exit(1);
    } break;
    }
}

void convertWavToFlacInDir(Path const& path)
{
    using namespace std::filesystem;
    for (directory_entry const& dir_entry : directory_iterator(path)) {
        convertDirEntryToFlac(dir_entry);
    }
}

void convertWavToFlacInDirTree(Path const& path)
{
    using namespace std::filesystem;
    for (directory_entry const& dir_entry : recursive_directory_iterator(path)) {
        convertDirEntryToFlac(dir_entry);
    }
}

} // namespace auconv