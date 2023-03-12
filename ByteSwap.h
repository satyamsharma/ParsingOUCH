#pragma once

// Multibyte swap
inline uint16_t bswap(uint16_t value) noexcept
{
    value = (value >> 8) | (value << 8);
    return value;
}

inline uint32_t bswap(uint32_t value) noexcept
{
    uint32_t tmp = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
    value = (tmp << 16) | (tmp >> 16);
    return value;
}