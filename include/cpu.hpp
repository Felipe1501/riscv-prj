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
    
    public:
        explicit CPU(Bus& b);

        void reset();
        void dump() const;
        void step();
};