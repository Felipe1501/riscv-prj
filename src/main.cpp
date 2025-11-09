#include <iostream>
#include "memory.hpp"
#include "bus.hpp"
#include "cpu.hpp"

uint32_t encodeADDI(uint8_t rd, uint8_t rs1, int32_t imm){
    uint32_t opcode = 0x13;
    uint32_t funct3 = 0x0;
    uint32_t imm12 = static_cast<uint32_t>(imm & 0xFFF);

    return (imm12 << 20)
        | (static_cast<uint32_t>(rs1) << 15)
        | (funct3 << 12)
        | (static_cast<uint32_t>(rd) << 7)
        | opcode;
}

int main() {
    std::cout << "Iniciando teste (cpu ADDI)" << std::endl;

    Memory mem(64);
    Bus bus(mem);   
    CPU cpu(bus);

    uint32_t inst0 = encodeADDI(1, 0, 5);
    uint32_t inst1 = encodeADDI(2, 1, 3);
    uint32_t inst2 = encodeADDI(3, 2, -1);

    mem.write32(0, inst0);
    mem.write32(4, inst1);
    mem.write32(8, inst2);

    std::cout << "\nDump inicial da CPU:\n";
    cpu.dump();

    std::cout << "\nExecutando 3 instrucoes:\n";
    cpu.step();
    cpu.step();
    cpu.step();

    std::cout << "\nDump final da CPU:\n";
    cpu.dump();

    return 0;
}