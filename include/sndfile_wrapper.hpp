// sndfile_wrapper.hpp - auconv

// This wrapper converts unsafe buffer operations in libsnfile to safe buffer
//   operations on well-defined types.

#ifndef AUCONV_SNDFILE_WRAPPER_HPP
#define AUCONV_SNDFILE_WRAPPER_HPP

#include <sndfile.hh>

#include <defines.hpp>

namespace auconv {

class SndfileHandleHandle {
public:
    template <typename T, sf_count_t N>
    sf_count_t read(std::array<T, N> buffer);

    template <typename T, sf_count_t N>
    sf_count_t write(std::array<T, N> buffer);

    template <typename T, sf_count_t N>
    sf_count_t readf(std::array<T, N> buffer);

    template <typename T, sf_count_t N>
    sf_count_t writef(std::array<T, N> buffer);

private:
    SndfileHandle handle_;
};

} // namespace auconv

#endif // AUCONV_SNDFILE_WRAPPER_HPP