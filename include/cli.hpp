// cli.hpp - auconv

#ifndef AUCONV_CLI_HPP
#define AUCONV_CLI_HPP

#include <string>
#include <vector>

#include <defines.hpp>

namespace auconv {

constexpr std::string FLAG_MODE_FILE = "-s";
constexpr std::string FLAG_MODE_DIRECTORY = "-d";
constexpr std::string FLAG_MODE_DIRECTORY_TREE = "-t";

constexpr std::string FLAG_MODE_FILE_QUOTED = "'-s'";
constexpr std::string FLAG_MODE_DIRECTORY_TREE_QUOTED = "'-t'";
constexpr std::string FLAG_MODE_DIRECTORY_QUOTED = "'-d'";

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

ParsedArgs handleCLIArguments(std::vector<std::string> const& args);

} // namespace auconv

#endif // AUCONV_CLI_HPP