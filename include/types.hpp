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
using ArgArray = std::vector<std::string>;
using Path     = std::filesystem::path;

// Specifies the type of path; implicitly specifies what to convert
// TODO(MATT): May want to pry apart the type of path from the conversion mode...
//  ...but it may be redundant
enum struct PathType : int_fast8_t {
    Invalid,       // an invalid path, or an invalid conversion mode
    File,          // a single file
    Directory,     // all files in a directory
    DirectoryTree, // all files in a directory tree, recursively
};

// Specifies an input file format
enum struct Format : int_fast8_t {
    null,
    all, // convert all decodable audio files to the output format
    wav,
    flac,
    mp3,
    // ...
};

struct InFormat {
    Format fmt_;

    InFormat() = delete;
    explicit InFormat(Format fmt)
        : fmt_ {fmt}
    { }
};

struct OutFormat {
    Format fmt_;

    OutFormat() = delete;
    explicit OutFormat(Format fmt)
        : fmt_ {fmt}
    { }
};

struct ParsedArgs {
    Path      path; // conversion target path
    PathType  mode {PathType::File};
    InFormat  in_format {Format::all};
    OutFormat out_Format {Format::flac};
};

} // namespace auconv

#endif // AUCONV_TYPES_HPP