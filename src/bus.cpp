#include "bus.hpp"

Bus::Bus(Memory& mem) 
        : memory(mem) {}

uint8_t Bus::read8(uint32_t address) const {
    std::cout << "Bus: lendo byte endereco " << address << std::endl;
    return memory.read8(address);
}

void Bus::write8(uint32_t address, uint8_t value) {
    std::cout << "Bus: escrevendo byte " << (int)value 
              << " no endereco " << address << std::endl;
    memory.write8(address, value);
}

uint32_t Bus::read32(uint32_t address) const {
    std::cout << "Bus: lendo palavra 32 bits no endereco " << address << std::endl;
    return memory.read32(address);
}

void Bus::write32(uint32_t address, uint32_t value) {
    std::cout << "Bus: escrevendo palavra 32 bits= " 
              << value << " no endereço " << address << std::endl;
    memory.write32(address, value);
}