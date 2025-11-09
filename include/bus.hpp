#pragma once
#include "memory.hpp"
#include <cstdint>
#include <iostream>

class Bus{
    private:
        Memory& memory;
    
    public:
        explicit Bus(Memory& mem);

        uint8_t read8(uint32_t address) const;
        void write8(uint32_t address, uint8_t value);

        uint32_t read32(uint32_t address) const;
        void write32(uint32_t address, uint32_t value);

};
       