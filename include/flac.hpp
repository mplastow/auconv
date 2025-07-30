// flac.hpp - auconv

#ifndef AUCONV_FLAC_HPP
#define AUCONV_FLAC_HPP

#include <defines.hpp>
#include <types.hpp>

namespace auconv {

void convertToFlacFile(const Path& input, const Path& output, int out_format);

void convertWavFilesToFlac(const Path& path, PathType path_type);

void convertWavToFlacInDir(const Path& path);

void convertWavToFlacInDirTree(const Path& path);

} // namespace auconv

#endif // AUCONV_FLAC_HPP