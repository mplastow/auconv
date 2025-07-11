// defines.hpp - auconv

#ifndef AUCONV_DEFINES_HPP
#define AUCONV_DEFINES_HPP

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace auconv {

constexpr size_t BUFFER_SIZE = 1024;
constexpr size_t MP3_BUFFER_SIZE = BUFFER_SIZE;
constexpr size_t PCM_BUFFER_SIZE = BUFFER_SIZE * 2;

// NOTE(MATT): The following defines are equivalent to type `int` because that's what lame takes
constexpr int_fast32_t HZ_44100 = 44100;
constexpr int_fast32_t BITRATE_320 = 320;
constexpr int_fast32_t MP3_COMP_RATIO_DEFAULT = 11;

// Commonly used type aliases
using ArgVec = std::vector<std::string>;
using Path = std::filesystem::path;

} // namespace auconv

#endif // AUCONV_DEFINES_HPP