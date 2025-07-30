// flac.hpp - auconv

#ifndef AUCONV_FLAC_HPP
#define AUCONV_FLAC_HPP

#include <defines.hpp>
#include <types.hpp>

namespace auconv {

void convertWavToFlacFile(Path const& input, Path const& output, int out_format);

void convertWavToFlacInDirs(Path const& path, PathType path_type);

void convertWavToFlacInDir(Path const& path);

void convertWavToFlacInDirTree(Path const& path);

} // namespace auconv

#endif // AUCONV_FLAC_HPP