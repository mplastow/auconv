// files.hpp - auconv

#ifndef AUCONV_FILES_HPP
#define AUCONV_FILES_HPP

#include <sndfile.hh>

#include <defines.hpp>

namespace auconv {

void printFileInfo(Path const& input_file);
void printFileInfo(SndfileHandle const& handle, Path const& path);
void printOutputFileError(SndfileHandle const& in_handle, SndfileHandle const& out_handle, int format);

} // namespace auconv

#endif // AUCONV_FILES_HPP