// snd.hpp - auconv

#ifndef AUCONV_SND_HPP
#define AUCONV_SND_HPP

#include <sndfile.h>
#include <sndfile.hh>

#include <defines.hpp>
#include <files.hpp>
#include <types.hpp>

namespace auconv {

void handleParsedArgs(ParsedArgs const& args);

} // namespace auconv

#endif // AUCONV_SND_HPP