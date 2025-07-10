// cli.cpp - auconv

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <cli.hpp>
#include <flac.hpp>
#include <snd.hpp>

namespace auconv {

using ArgVec = std::vector<std::string>;
using Path = std::filesystem::path;

namespace {

    // TODO(MATT): Break this massive function up into comprehensible parts
    void beginAudioConversion(ArgVec const& args)
    {
        std::string mode_flag { args[1] };
        Path user_path { args[2] };

        auto starting_path = std::filesystem::current_path();
        if (user_path != ".") {
            starting_path = user_path;
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
    }

    void printArgs(ArgVec const& args)
    {
        std::cout << "CLI ARGS: \n";
        size_t argc { 0 };
        for (auto const& arg : args) {
            std::cout << "args[" << argc++ << "]: \" " << arg << " \"\n";
        }
        std::cout << "END OF CLI ARGS\n\n";
    }

    void printHelp()
    {
        std::cout << "auconv v0.01: a basic WAV to FLAC converter" << '\n';
        std::cout << "Usage: auconv [mode] [path]" << '\n';
        std::cout << "Modes:" << '\n';
        std::cout << "\t-s: convert single file. [path] is path to file." << '\n';
        std::cout << "\t-d: convert all files in a single directory. [path] is path to directory." << '\n';
        std::cout << "\t-t: convert all files in a directory and all its subdirectories. [path] is path to top directory." << '\n';
        std::cout << "Path: must be absolute path, e.g. /home/user/audio/wav/, or '.' for current directory\n"
                  << '\n';

        std::quick_exit(0);
    }

    void wrongArgNumber()
    {
        std::cout << "Incorrect number of arguments. Use auconv --help" << '\n';
        std::quick_exit(1);
    }

} // namespace

// TODO(MATT): Rather than doing all work immediately, return a parsed CLI arg object
//  to the callsite and let the callsite handle it.
ParsedArgs parseCLIArguments(ArgVec const& args)
{
    printArgs(args);

    switch (args.size()) {
    case 2: {
        if (args.at(1) == "--help") {
            printHelp();
        } else {
            wrongArgNumber();
        }
    } break;
    case 3: {
        beginAudioConversion(args);
    } break;

    default: {
        wrongArgNumber();
    }
    }

    return ParsedArgs {};
}

} // namespace auconv
