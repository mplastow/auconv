// files.hpp - auconv

#ifndef AUCONV_FILES_HPP
#define AUCONV_FILES_HPP

#include <filesystem>

// #include <sndfile.h>
#include <sndfile.hh>

namespace auconv {

void printFileInfo(std::filesystem::path const& inputFile);

} // namespace auconv

#endif // AUCONV_FILES_HPP