// flac.hpp - auconv

#ifndef AUCONV_FLAC_HPP
#define AUCONV_FLAC_HPP

#include <defines.hpp>
#include <types.hpp>

namespace auconv {

void convertDirEntryToFlac(std::filesystem::directory_entry const& dir_entry);

} // namespace auconv

#endif // AUCONV_FLAC_HPP