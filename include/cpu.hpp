#pragma once
#include "bus.hpp"
#include <array>
#include <cstdint>
#include <iostream>

class CPU {
    private:
        std::array<uint32_t, 32> x{};
        uint32_t pc{0};
        Bus& bus;
    

        uint32_t getBits(uint32_t value, int hi, int lo) const;
        int32_t signExtend(uint32_t value, int bits) const;

    public:
        explicit CPU(Bus& b);

        void reset();
        void dump() const;
        void step();
};