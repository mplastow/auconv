// flac.hpp - auconv

#ifndef AUCONV_FLAC_HPP
#define AUCONV_FLAC_HPP

#include <filesystem>

namespace auconv {

void convertWavToFlacInDir(std::filesystem::path const& starting_path);

void convertWavToFlacInDirTree(std::filesystem::path const& starting_path);

} // namespace auconv

#endif // AUCONV_FLAC_HPP