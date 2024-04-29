#pragma once

#include <cstdint>
#include <cstddef>

namespace arch
{
    class ARCH_INFO {
    public:
    #ifdef __SSE__
    constexpr static const bool SSE = true;
    #else
    constexpr static const bool SSE = false;
    #endif
    #ifdef __SSE2__
    constexpr static const bool SSE2 = true;
    #else
    constexpr static const bool SSE2 = false;
    #endif
    #ifdef __SSE3__
    constexpr static const bool SSE3 = true;
    #else
    constexpr static const bool SSE3 = false;
    #endif
    #ifdef __AVX512F__
    constexpr static const bool AVX512F = true;
    #else
    constexpr static const bool AVX512F = false;
    #endif
    #ifdef __AVX2__
    constexpr static const bool AVX2 = true;
    #else
    constexpr static const bool AVX2 = false;
    #endif
    #ifdef __AVX__
    constexpr static const bool AVX = true;
    #else
    constexpr static const bool AVX = false;
    #endif
    
    constexpr static const size_t SIMDFlag = 
        1ULL * SSE + 
        2ULL * SSE2 + 
        4ULL * SSE3 + 
        8ULL * AVX + 
        16ULL * AVX2 + 
        32ULL * AVX512F;
};
} // namespace arch
