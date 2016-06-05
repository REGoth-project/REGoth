#pragma once

namespace Memory
{
    /**
     * Returns the number of bits needed to store the number x, at compile time
     */
    constexpr unsigned numberOfBits(unsigned x)
    {
        return x < 2 ? x : 1+numberOfBits(x >> 1);
    }
}