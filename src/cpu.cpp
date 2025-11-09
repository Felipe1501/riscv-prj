#include "cpu.hpp"

CPU::CPU(Bus& b) 
    : bus(b) {
    reset();
}

void CPU::reset() {
    x.fill(0);
    pc = 0;
    std::cout << "CPU resetada. \n";
}

void CPU::dump() const {
    std::cout << "[CPU] Estado atual:\n";
    std::cout << "PC = " << pc << std::endl;
    for (int i = 0; i < 8; i++){
        std::cout << "x" << i << " = " << x[i] << "\t";
    }
    std::cout << std::endl;
}

uint32_t CPU::getBits(uint32_t value, int hi, int lo) const {
    uint32_t mask = ((1u << (hi - lo + 1)) - 1u);
    return (value >> lo) & mask;
}

int32_t CPU::signExtend(uint32_t value, int bits) const{
    int32_t s = static_cast<int32_t>(value);
    int shift = 32 - bits;
    return (s << shift) >> shift;
}

void CPU::step(){
    std::cout << "[CPU] Executando ciclo no @PC = " << pc << std::endl;

    uint32_t inst = bus.read32(pc);
    std::cout << "[CPU] Instrucao lida: 0x" << std::hex << inst << std::dec << std::endl;

    uint32_t opcode = getBits(inst, 6, 0);
    uint32_t rd     = getBits(inst, 11, 7);
    uint32_t funct3 = getBits(inst, 14, 12);
    uint32_t rs1    = getBits(inst, 19, 15);
    uint32_t imm12  = getBits(inst, 31, 20);
    int32_t imm = signExtend(imm12, 12);


    std::cout << "[CPU] opcode= " << opcode 
              << ", rd=" << rd 
              << ", funct3=" << funct3 
              << ", rs1=" << rs1 
              << ", imm=" << imm << std::endl;
    
    if (opcode == 0x13 && funct3 == 0x0) {
        uint32_t old = x[rs1];
        uint32_t result = old + imm;

        if (rd != 0) {
            x[rd] = result;
        }

        std::cout << "[CPU] Executando ADDI: x" << rd 
                  << " = x" << rs1 << "(" << old << ") + "
                  << imm << " -> " << result << std::endl;
    } else {
        std::cout << "[CPU] Instrucao desconhecida. NOP.\n";
    }
        
    pc += 4;

    x[0] = 0;
}