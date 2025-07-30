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

    /// TODO: (MATT)  multiformat
    void convertFile(const Path& path)
    {
        if (path.extension() == ".wav") {
            std::cout << "Converting wav file: " << path.filename() << '\n';

            std::string output = path.parent_path();
            output.append("/").append(path.stem()).append(".flac");

            /// TODO: (MATT)  multiformat, don't hardcode this!
            auconv::convertToFlacFile(path, Path {output}, SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
        } else {
            std::cout << "Skipping due to file extension: " << path.filename() << '\n';
        }
    }

    void convertFile(const Path& path, InFormat in_format, OutFormat out_format)
    {
        if (in_format.fmt_ == out_format.fmt_) {
            std::cout << "Skipping " << path.filename() << " because input format matches output format.\n";
        }

        switch (in_format.fmt_) {
        case (Format::wav): {
            std::cout << "Converting wav file: " << path.filename() << '\n';

            std::string output = path.parent_path();
            output.append("/").append(path.stem()).append(".flac");

            /// TODO: (MATT)  multiformat, don't hardcode this!
            auconv::convertToFlacFile(path, Path {output}, SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
        } break;
        default: {
            std::cout << "Skipping due to file extension: " << path.filename() << '\n';
        }
        }
    }

    void convertFiles(const Path& path, PathType path_type)
    {
        convertWavFilesToFlac(path, path_type);
    }

    void convertFilesNew(const Path& path, PathType path_type)
    {
        convertWavFilesToFlac(path, path_type);
    }

} // namespace

void handleParsedArgs(ParsedArgs const& args)
{
    /// TODO: (MATT) multiformat
    convertFiles(args.path, args.mode);
}

} // namespace auconv