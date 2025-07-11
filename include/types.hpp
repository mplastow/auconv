// types.hpp - auconv

#ifndef AUCONV_TYPES_HPP
#define AUCONV_TYPES_HPP

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <defines.hpp>

namespace auconv {
// Type aliases
using ArgVec = std::vector<std::string>;
using Path = std::filesystem::path;

// Specifies the type of path; implicitly specifies what to convert
// TODO(MATT): May want to pry apart the type of path from the conversion mode...
//  ...but it may be redundant
enum struct PathType : int_fast8_t {
    Invalid,       // an invalid path, or an invalid conversion mode
    File,          // a single file
    Directory,     // all files in a directory
    DirectoryTree, // all files in a directory tree, recursively
};

// Specifies the file format that should be converted
enum struct InFormat : int_fast8_t {
    null,
    all, // convert all decodable audio files to the output format
    wav,
    flac,
    mp3,
    // ...
};

// Specifies the output file format for all converted files
enum struct OutFormat : int_fast8_t {
    null,
    wav,
    flac,
    mp3,
    // ...
};

struct ParsedArgs {
    Path path; // conversion target path
    PathType mode = PathType::File;
    InFormat in_format = InFormat::all;
    OutFormat out_Format = OutFormat::flac;
};

} // auconv

#endif // AUCONV_TYPES_HPP