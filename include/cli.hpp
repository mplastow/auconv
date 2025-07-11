// cli.hpp - auconv

#ifndef AUCONV_CLI_HPP
#define AUCONV_CLI_HPP

#include <string>

#include <defines.hpp>
#include <types.hpp>

namespace auconv {

constexpr std::string FLAG_MODE_FILE = "-s";
constexpr std::string FLAG_MODE_DIRECTORY = "-d";
constexpr std::string FLAG_MODE_DIRECTORY_TREE = "-t";

constexpr std::string FLAG_MODE_FILE_QUOTED = "'-s'";
constexpr std::string FLAG_MODE_DIRECTORY_TREE_QUOTED = "'-t'";
constexpr std::string FLAG_MODE_DIRECTORY_QUOTED = "'-d'";

ParsedArgs handleCLIArguments(ArgVec const& args);

} // namespace auconv

#endif // AUCONV_CLI_HPP