// cli.cpp - auconv

#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cli.hpp>
#include <flac.hpp>
#include <snd.hpp>

namespace auconv {

using ArgVec = std::vector<std::string>;
using Path = std::filesystem::path;

namespace {

    // Got a certain path type, but want another
    void wrongPathType(PathType got, PathType want)
    {
        // Reflection would be good for this task
        std::string_view mode_str {};
        switch (want) {
        case PathType::File: {
            mode_str = "'-s'";
        } break;
        case PathType::Directory: {
            mode_str = "'-d'";
        } break;
        case PathType::DirectoryTree: {
            mode_str = "'-t'";
        }
        }

        switch (got) {
        case PathType::File: {
            std::cout << "Path must be a file, not a directory, when using " << mode_str << '\n';
        } break;
        default: {
            std::cout << "Path must be a directory, not a file, when using " << mode_str << '\n';
        }
        }
    };

    void convertFile(Path const& path)
    {
        if (!path.has_filename()) {
            wrongPathType(PathType::Directory, PathType::File);
        }

        if (path.extension() == ".wav") {
            std::cout << "Converting wav file: " << path.filename() << '\n';

            std::string outputFile = path.parent_path();
            outputFile.append("/").append(path.stem()).append(".flac");

            auconv::convertAudioFile(
                path,
                outputFile,
                SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
        }
    }

    void convertDirectory(Path const& path)
    {
        if (path.has_filename()) {
            wrongPathType(PathType::File, PathType::Directory);
        }

        auconv::convertWavToFlacInDir(path);
    }

    void convertDirectoryTree(Path const& path)
    {
        if (path.has_filename()) {
            wrongPathType(PathType::File, PathType::DirectoryTree);
        }

        auconv::convertWavToFlacInDirTree(path);
    }

    ParsedArgs parseArgs(ArgVec const& args)
    {
        std::string const& mode { args[1] };
        Path path { args[2] };

        // TODO(MATT): Test whether this is a redundant operation
        if (path == ".") {
            path = std::filesystem::current_path();
        }

        if (!std::filesystem::exists(path)) {
            std::cout << "The path you provided is not valid." << '\n';
            std::quick_exit(1);
        }

        // TODO(MATT): Don't dispatch work from here; parsed to ParsedArgs instead
        if (mode == "-s") {
            convertFile(path);
        } else if (mode == "-d") {
            convertDirectory(path);
        } else if (mode == "-t") {
            convertDirectoryTree(path);
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

ParsedArgs handleCLIArguments(ArgVec const& args)
{
    printArgs(args); // TODO(MATT): get rid of this eventually

    switch (args.size()) {
    case 2: {
        if (args.at(1) == "--help") {
            printHelp();
        } else {
            wrongArgNumber();
        }
    } break;
    case 3: {
        return parseArgs(args);
    } break;

    default: {
        wrongArgNumber();
    }
    }

    std::unreachable();
}

} // namespace auconv
