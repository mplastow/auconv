// snd.cpp - auconv

#include <snd.hpp>

#include <filesystem>
#include <iostream>

#include <cli.hpp>
#include <defines.hpp>
#include <flac.hpp>
#include <types.hpp>

namespace auconv {

namespace {

    // TODO(MATT): multiformat
    void convertFile(Path const& path)
    {

        if (path.extension() == ".wav") {
            std::cout << "Converting wav file: " << path.filename() << '\n';

            std::string output = path.parent_path();
            output.append("/").append(path.stem()).append(".flac");

            // TODO(MATT): multiformat, don't hardcode this!
            auconv::convertWavToFlacFile(path, Path { output }, SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
        } else {
            std::cout << "Skipping due to file extension: " << path.filename() << '\n';
        }
    }

    // TODO(MATT): multiformat
    void convertDirectory(Path const& path)
    {
        // TODO(MATT): multiformat, don't hardcode this!
        auconv::convertWavToFlacInDir(path);
    }

    // TODO(MATT): multiformat
    void convertDirectoryTree(Path const& path)
    {
        // TODO(MATT): multiformat, don't hardcode this!
        auconv::convertWavToFlacInDirTree(path);
    }

} // namespace

void handleParsedArgs(ParsedArgs const& args)
{
    switch (args.mode) {
    case PathType::File: {
        convertFile(args.path);
    } break;
    case PathType::Directory: {
        convertDirectory(args.path);
    } break;
    case PathType::DirectoryTree: {
        convertDirectoryTree(args.path);
    } break;
    case PathType::Invalid: {
        std::quick_exit(1);
    } break;
    default: {
        std::quick_exit(1);
    }

        std::cout << "Converted all audio files." << '\n';
    }
}

} // namespace auconv