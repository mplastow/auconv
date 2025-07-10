// cli.hpp - auconv

#ifndef AUCONV_CLI_HPP
#define AUCONV_CLI_HPP

#include <filesystem>
#include <string>
#include <vector>

namespace auconv {

enum class OperatingMode : int8_t {
    File,         // single file
    Directory,    // entire directory
    DirectoryTree // entire directory tree, recursively
};

struct ParsedArgs {
    std::filesystem::path path; // conversion target path
    OperatingMode mode = OperatingMode::File;
};

ParsedArgs parseCLIArguments(std::vector<std::string> const& args);

} // namespace auconv

#endif // AUCONV_CLI_HPP