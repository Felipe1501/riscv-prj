#include "memory.hpp"

Memory::Memory(std::size_t size) 
        : data(size, 0) {}

uint8_t Memory::read8(std::size_t address) const {
    return data.at(address);
}

void Memory::write8(std::size_t address, uint8_t value) {
    data.at(address) = value;
}

uint32_t Memory::read32(std::size_t address) const {
    uint32_t b0 = data.at(address);
    uint32_t b1 = data.at(address + 1);
    uint32_t b2 = data.at(address + 2);
    uint32_t b3 = data.at(address + 3);
    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

void Memory::write32(std::size_t address, uint32_t value) {
    data.at(address)     = static_cast<uint8_t>(value & 0xFF);
    data.at(address + 1) = static_cast<uint8_t>((value >> 8) & 0xFF);
    data.at(address + 2) = static_cast<uint8_t>((value >> 16) & 0xFF);
    data.at(address + 3) = static_cast<uint8_t>((value >> 24) & 0xFF);

}

void Memory::dump(std::size_t start, std::size_t count) const{
    std::cout << "Dump de Memoria [" << start << " .. " << (start + count - 1) << "]: ";
    for (std::size_t i = 0; i < count; ++i) {
        std::cout << (int)data.at(start + i) << " ";
    }
    std::cout << std::endl;
}