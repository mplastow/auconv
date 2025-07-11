// mp3.hpp - auconv

#ifndef AUCONV_MP3_HPP
#define AUCONV_MP3_HPP

#include <lame/lame.h>

#include <defines.hpp>

namespace auconv {

lame_global_flags* initLameWithFlags();

void convertWavToMp3(Path const& inputFile, Path const& outputFile);

} // namespace auconv

#endif // AUCONV_MP3_HPP