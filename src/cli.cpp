// cli.cpp - auconv

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <utility>

#include <cli.hpp>
#include <defines.hpp>
#include <snd.hpp>
#include <types.hpp>

namespace auconv {

namespace {

    // Validate that we got a valid path of the type we want, given the mode flag
    // Arg name intuition: Here's the `path` we `got`, this is the type we `want`
    PathType validatePath(const Path& path, PathType want)
    {
        std::string_view mode_str {};
        auto             got {PathType::Invalid};
        switch (want) {

            /// TODO: (MATT)  How much of this code is actually redundant?
        case PathType::File: {
            if (std::filesystem::is_regular_file(path)) {
                return PathType::File;
            } else if (std::filesystem::is_directory(path)) {
                got      = PathType::Directory;
                mode_str = FLAG_MODE_FILE_QUOTED;
            } else {
                return PathType::Invalid;
            }
        } break;

        case PathType::Directory: {
            if (std::filesystem::is_directory(path)) {
                return PathType::Directory;
            } else if (std::filesystem::is_regular_file(path)) {
                got      = PathType::File;
                mode_str = FLAG_MODE_DIRECTORY_QUOTED;
            } else {
                return PathType::Invalid;
            }
        } break;

        case PathType::DirectoryTree: {
            if (std::filesystem::is_directory(path)) {
                return PathType::DirectoryTree;
            } else if (std::filesystem::is_regular_file(path)) {
                got      = PathType::File;
                mode_str = FLAG_MODE_DIRECTORY_TREE_QUOTED;
            } else {
                return PathType::Invalid;
            }
        } break;

        case PathType::Invalid: {
            std::cout << "Unspecified conversion mode. Use auconv --help" << '\n';
            std::quick_exit(1);
        } break;
        }

        switch (got) {
        case PathType::File: {
            std::cout << "Path must be a file, not a directory, when using " << mode_str << '\n';
        } break;
        default: {
            std::cout << "Path must be a directory, not a file, when using " << mode_str << '\n';
        }
        }

        std::quick_exit(1);
    }

    PathType parseMode(const Path& path, std::string_view mode)
    {
        PathType path_type {PathType::Invalid};
        if (mode == FLAG_MODE_FILE) {
            path_type = validatePath(path, PathType::File);
        } else if (mode == FLAG_MODE_DIRECTORY) {
            path_type = validatePath(path, PathType::Directory);
        } else if (mode == FLAG_MODE_DIRECTORY_TREE) {
            path_type = validatePath(path, PathType::DirectoryTree);
        } else {
            std::cout << "Mode is not valid. Use auconv --help\n";
            std::quick_exit(1);
        }
        return path_type;
    }

    CLIArgs parseArgs(ArgArray const& args)
    {
        Path path {args[2]};

        /// TODO: (MATT) Test whether this is a redundant operation
        if (!std::filesystem::exists(path)) {
            std::cout << "The provided path is not valid." << '\n';
            std::quick_exit(1);
        }

        CLIArgs parsed_args {.path {path}, .mode {}};

        std::string_view mode {args[1]};
        parsed_args.mode = parseMode(path, mode);

        /// TODO: (MATT)  Parse args for input and output types

        return parsed_args;
    }

    void printArgs(ArgArray const& args)
    {
        std::cout << "CLI ARGS: \n";
        size_t argc {0};
        for (auto const& arg : args) {
            std::cout << "args[" << argc++ << "]: \" " << arg << " \"\n";
        }
        std::cout << "END OF CLI ARGS\n\n";
    }

    void printErrorArgNumber()
    {
        std::cout << "Incorrect number of arguments. Use auconv --help" << '\n';
        std::quick_exit(1);
    }

    void printHelp()
    {
        std::cout << "auconv v0.01: a basic WAV to FLAC converter" << '\n';
        std::cout << "Usage: auconv [mode] [path]" << '\n';
        std::cout << "Modes:" << '\n';
        std::cout << "\t-s, -f: convert single file. [path] is path to file." << '\n';
        std::cout << "\t-d: convert all files in a single directory. [path] is path to directory." << '\n';
        std::cout << "\t-t: convert all files in a directory and all its subdirectories. [path] is path to top directory." << '\n';
        std::cout << "Path: must be absolute path, e.g. /home/user/audio/wav/, or '.' for current directory\n"
                  << '\n';

        std::quick_exit(0);
    }

} // namespace

CLIArgs handleCLIArguments(ArgArray const& args)
{
    /// TODO: (MATT)  get rid of this eventually
    printArgs(args);

    switch (args.size()) {
    case 2: {
        if (args.at(1) == "--help") {
            printHelp();
        } else {
            printErrorArgNumber();
        }
    } break;
    case 3: {
        return parseArgs(args);
    } break;

    default: {
        printErrorArgNumber();
    }
    }

    std::unreachable();
}

} // namespace auconv
