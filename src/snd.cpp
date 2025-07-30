// snd.cpp - auconv

#include <cstdlib>
#include <snd.hpp>

#include <filesystem>
#include <iostream>

#include <cli.hpp>
#include <defines.hpp>
#include <types.hpp>

namespace auconv {

namespace {

    void doBufferReadWrite(SndfileHandle& in, SndfileHandle& out)
    { // Initialize conversion buffer
        std::array<short, BUFFER_SIZE> buffer {};

        // Read through file to convert from input format to output format
        while (true) {
            // Read a chunk of data into the buffer
            auto read = in.read(buffer.data(), buffer.size());

            // No more bytes to be read, so we're done
            if (read == 0) {
                std::cout << "Read no bytes\n";
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

    void doConversion(const Path& input, const Path& output, int format)
    // Precondition: input and output must not be the same path
    {
        // Get handle to input file
        SndfileHandle in_handle {input};
        if (in_handle.error() != 0) {
            std::cout << "Could not open input file: " << in_handle.strError() << '\n';
            return;
        } else {
            if (in_handle.format() == format) {
                std::cout << "Skipping file: input format same as output format.\n";
                return;
            }

            printFileInfo(input);
        }

        /// TODO: (MATT) Investigate how to set correct settings for wav and mp3
        // Get handle to output file and specify format
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

    void processDirEntry(std::filesystem::directory_entry const& dir_entry, int fmt, std::string_view ext)
    {
        if (dir_entry.is_regular_file()) {
            if (dir_entry.path().extension() == ".wav") {
                std::string outputFile = dir_entry.path().parent_path();
                outputFile.append("/").append(dir_entry.path().stem()).append(ext);

                if (dir_entry.path() != outputFile) {
                    std::cout << "Converting wav file: " << dir_entry.path().filename() << '\n';
                    doConversion(dir_entry.path(), outputFile, fmt);
                } else {
                    std::cout << "Input and output files are the same. Skipping file: " << dir_entry.path().filename() << "\n";
                }
            } else {
                std::cout << "Converting other file: " << dir_entry.path().filename() << '\n';

                std::string outputFile = dir_entry.path().parent_path();
                outputFile.append("/").append(dir_entry.path().stem()).append(ext);

                doConversion(dir_entry.path(), outputFile, fmt);
                // std::cout << "Skipping file: " << dir_entry.path().filename() << '\n';
            }
        } else {
            std::cout << "Directory: " << dir_entry << '\n';
        }
    }

    void sendConversionTask(
        const std::filesystem::directory_entry& dir_entry,
        // InFormat                                in_fmt  = InFormat {Format::wav},
        OutFormat out_fmt = OutFormat {Format::wav})
    {
        switch (out_fmt.fmt_) {
        case Format::flac: {
            processDirEntry(dir_entry, FORMAT_FLAC_PCM_16, ".flac");
        } break;
        case Format::mp3: {
            // processDirEntry(dir_entry, FORMAT_MP3, ".mp3");
            std::cout << "MP3 output not yet supported.\n";
            std::quick_exit(1);
        } break;
        case Format::wav: {
            processDirEntry(dir_entry, FORMAT_WAV_PCM_16, ".wav");
        } break;
        default: {
            std::cout << "Skipping due to file extension: " << dir_entry.path().filename() << '\n';
        } break;
        }
    }

} // namespace

void convertFiles(ParsedArgs const& args)
{
    using namespace std::filesystem;

    switch (args.mode) {
    case PathType::Directory: {
        for (directory_entry const& dir_entry : directory_iterator(args.path)) {
            sendConversionTask(dir_entry);
        }
    } break;
    case PathType::DirectoryTree: {
        for (directory_entry const& dir_entry : recursive_directory_iterator(args.path)) {
            sendConversionTask(dir_entry);
        }
    } break;
    case PathType::File: {
        sendConversionTask(directory_entry(args.path));
    } break;
    default: {
        std::cout << "Path is not valid.\n";
        std::quick_exit(1);
    } break;
    }
}

} // namespace auconv