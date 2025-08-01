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
enum struct FormatTag : int_fast8_t {
    null,
    all, // convert all decodable audio files to the output format
    wav,
    flac,
    mp3,
    // ...
};

struct InFormat {
    FormatTag fmt_;

    InFormat()
        : fmt_ {FormatTag::null}
    { }
    explicit InFormat(FormatTag fmt)
        : fmt_ {fmt}
    { }
};

struct OutFormat {
    FormatTag fmt_;

    OutFormat()
        : fmt_ {FormatTag::null}
    { }
    explicit OutFormat(FormatTag fmt)
        : fmt_ {fmt}
    { }
};

struct CLIArgs {
    Path      path; // conversion target path
    PathType  mode {PathType::File};
    InFormat  in_format {FormatTag::all};
    OutFormat out_Format {FormatTag::flac};
};

} // namespace auconv

#endif // AUCONV_TYPES_HPP