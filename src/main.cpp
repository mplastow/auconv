// main.cpp - auconv

#include <iostream>
#include <cstdio>
#include <string>
#include <filesystem>

#include <lame/lame.h>
#include <sndfile.hh>
#include <sndfile.h>

#include <defines.hpp>
#include <files.hpp>
#include <flac.hpp>
#include <mp3.hpp>
#include <snd.hpp>

namespace { // provides internal linkage for `main.cpp`

} // namespace

auto main(int argc, char* argv[]) -> int
{
    if (argc == 2) {
        std::string help_flag = argv[1];
        if (help_flag == "--help") {
            std::cout << "auconv v0.01: a basic WAV to FLAC converter" << '\n';
            std::cout << "Usage: auconv [mode] [path]" << '\n';
            std::cout << "Modes:" << '\n';
            std::cout << "\t-s: convert single file. [path] is path to file." << '\n';
            std::cout << "\t-d: convert all files in a single directory. [path] is path to directory." << '\n';
            std::cout << "\t-t: convert all files in a directory and all its subdirectories. [path] is path to top directory." << '\n';
            std::cout << "Path: must be absolute path, e.g. /home/user/audio/wav/, or '.' for current directory\n"
                      << '\n';

            std::quick_exit(0);
        } else {
            std::cout << "Incorrect number of arguments. Use auconv --help" << '\n';
            std::quick_exit(1);
        }
    }

    if (argc != 3) {
        std::cout << "Incorrect number of arguments. Use auconv --help" << '\n';
        std::quick_exit(1);
    }

    std::string mode_flag = argv[1];
    std::string user_path = argv[2];

    auto starting_path = std::filesystem::current_path();
    if (user_path != ".") {
        starting_path = std::filesystem::path(user_path);
    }

    if (!std::filesystem::exists(starting_path)) {
        std::cout << "Path does not exist." << '\n';
        std::quick_exit(1);
    }

    // Single file
    if (mode_flag == "-s") {
        if (!starting_path.has_filename()) {
            std::cout << "Path must be a file, not a directory." << '\n';
            std::quick_exit(1);
        }

        if (starting_path.extension() == ".wav") {
            std::cout << "Converting wav file: " << starting_path.filename() << '\n';

            std::string outputFile = starting_path.parent_path();
            outputFile.append("/").append(starting_path.stem()).append(".flac");

            auconv::convertAudioFile(
                starting_path,
                outputFile,
                SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
        }
    }
    // Directory, all items
    else if (mode_flag == "-d") {
        // Check if path is to a file
        if (starting_path.has_filename()) {
            std::cout << "Path must be a directory, not a file, when using '-d'" << '\n';
            std::quick_exit(1);
        }

        auconv::convertWavToFlacInDir(starting_path);
    }
    // Directory tree, all items
    else if (mode_flag == "-t") {
        // Check if path is to a file
        if (starting_path.has_filename()) {
            std::cout << "Path must be a directory, not a file, when using '-t'" << '\n';
            std::quick_exit(1);
        }

        auconv::convertWavToFlacInDirTree(starting_path);
    } else {
        std::cout << "Unspecified conversion mode. Use auconv --help" << '\n';
        std::quick_exit(1);
    }

    std::cout << "Converted all audio files." << '\n';

    std::quick_exit(0);
}