#pragma once
#include <vector>
#include <cstdint>
#include <iostream>

class Memory {
    private:
        std::vector<uint8_t> data;

    public:
        explicit Memory(std::size_t size); 

        uint8_t read8(std::size_t address) const;
        void write8(std::size_t address, uint8_t value);

        uint32_t read32(std::size_t address) const;
        void write32(std::size_t address, uint32_t value);

        void dump(std::size_t start, std::size_t count) const;
};