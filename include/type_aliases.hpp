// type_aliases.hpp

#ifndef TYPE_ALIASES_HPP
#define TYPE_ALIASES_HPP

// Dependencies
#include <cstdio>
#include <cstddef>
#include <cstdint>

// Type definitions
// NOTE(matt): When writing idiomatic C++, use the preferred types noted below

using byte = __int8_t; // Use for explicitness when dealing with raw bytes, prefer std::byte

using isize = ssize_t; // All sizes "should be" (?) signed (isize)
using usize = size_t; // All sizes "should be" (?) signed (isize), prefer std::size_t

using iptr = intptr_t; // Use for computing address offsets that might be negative, prefer std::intptr_t
using uptr = uintptr_t; // Use for general address handling, prefer std::ptrdiff_t
using pdiff = ptrdiff_t; // Use for pointer arithmetic and array indexing, prefer std::ptrdiff_t

using u8 = __uint8_t;
using u16 = __uint16_t;
using u32 = __uint32_t;
using u64 = __uint64_t;
using u128 = __uint128_t;

using i8 = __int8_t;
using i16 = __int16_t;
using i32 = __int32_t;
using i64 = __int64_t;
using i128 = __int128_t;

using f16 = _Float16;
using f32 = _Float32;
using f64 = _Float64;
using f80 = _Float64x;
using f128 = __float128;

using b32 = __int32_t; // 32-bit Boolean for API compatability

using c8 = char8_t; // Standard char
using c16 = char16_t; // Wide char for UTF-16
using c32 = char32_t; // Extra wide char for UTF-32

#define U64MAX UINT64_MAX
#define I64MAX INT64_MAX
#define I64MIN INT64_MIN

static constexpr size_t KILOBYTE = 1024;
static constexpr size_t MEGABYTE = KILOBYTE * KILOBYTE;
static constexpr size_t GIGABYTE = MEGABYTE * KILOBYTE;

#endif // TYPE_ALIASES_HPP
