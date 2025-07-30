// snd.hpp - auconv

#ifndef AUCONV_SND_HPP
#define AUCONV_SND_HPP

#include <sndfile.h>
#include <sndfile.hh>

#include <defines.hpp>
#include <files.hpp>
#include <types.hpp>

namespace auconv {

// libsndfile audio formats are `int`s
constexpr int FORMAT_FLAC_PCM_16 {SF_FORMAT_FLAC | SF_FORMAT_PCM_16};

/// TODO: Does this work?
constexpr int FORMAT_MP3 {SF_FORMAT_MPEG | SF_FORMAT_MPEG_LAYER_III};

constexpr int FORMAT_WAV_PCM_16 {SF_FORMAT_WAV | SF_FORMAT_PCM_16};
constexpr int FORMAT_WAV_PCM_24 {SF_FORMAT_WAV | SF_FORMAT_PCM_24};

// Funcs

void convertFiles(ParsedArgs const& args);

} // namespace auconv

#endif // AUCONV_SND_HPP