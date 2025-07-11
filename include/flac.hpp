// flac.hpp - auconv

#ifndef AUCONV_FLAC_HPP
#define AUCONV_FLAC_HPP

#include <filesystem>

namespace auconv {

using Path = std::filesystem::path;

void convertWavToFlacFile(Path const& input, Path const& output, int out_format);

void convertWavToFlacInDir(Path const& path);

void convertWavToFlacInDirTree(Path const& path);

} // namespace auconv

#endif // AUCONV_FLAC_HPP