// mp3.hpp - auconv

#ifndef AUCONV_MP3_HPP
#define AUCONV_MP3_HPP

#include <lame/lame.h>

#include <defines.hpp>
#include <types.hpp>

namespace auconv {

lame_global_flags* initLameWithFlags();

void convertWavToMp3(Path const& input, Path const& output);

} // namespace auconv

#endif // AUCONV_MP3_HPP