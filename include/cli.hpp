// cli.hpp - auconv

#ifndef AUCONV_CLI_HPP
#define AUCONV_CLI_HPP

#include <filesystem>
#include <string>
#include <vector>

namespace auconv {

// Specifies the type of path; implicitly specifies what to convert
enum struct PathType : int_fast8_t {
    File,         // a single file
    Directory,    // all files in a directory
    DirectoryTree // all files in a directory tree, recursively
};

struct ParsedArgs {
    std::filesystem::path path; // conversion target path
    PathType mode = PathType::File;
};

ParsedArgs handleCLIArguments(std::vector<std::string> const& args);

} // namespace auconv

#endif // AUCONV_CLI_HPP