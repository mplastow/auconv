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

    void doConvertFile(
        const std::filesystem::directory_entry& dir_entry,
        // InFormat                                in_fmt  = InFormat {Format::wav},
        OutFormat out_fmt = OutFormat {Format::flac})
    {
        switch (out_fmt.fmt_) {
        case Format::flac: {
            convertDirEntryToFlac(dir_entry);
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
            doConvertFile(dir_entry);
        }
    } break;
    case PathType::DirectoryTree: {
        for (directory_entry const& dir_entry : recursive_directory_iterator(args.path)) {
            doConvertFile(dir_entry);
        }
    } break;
    case PathType::File: {
        doConvertFile(directory_entry(args.path));
    } break;
    default: {
        std::cout << "Cannot convert a file that doesn't exist!\n";
        std::quick_exit(1);
    } break;
    }
}

} // namespace auconv