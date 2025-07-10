// snd.hpp - auconv

#ifndef AUCONV_SND_HPP
#define AUCONV_SND_HPP

#include <sndfile.h>
#include <sndfile.hh>

#include <defines.hpp>
#include <files.hpp>

namespace auconv {

void convertAudioFile(
    std::filesystem::path const& inputFile,
    std::filesystem::path const& outputFile,
    int outputFormat);

} // namespace auconv

#endif // AUCONV_SND_HPP