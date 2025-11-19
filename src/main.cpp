#include <iostream>
#include <cstdint>
#include "memory.hpp"
#include "bus.hpp"
#include "cpu.hpp"

// ===== Encoders =====

// I-type: addi rd, rs1, imm  (opcode 0x13, funct3=0)
uint32_t encodeADDI(uint8_t rd, uint8_t rs1, int32_t imm) {
    const uint32_t opcode = 0x13, funct3 = 0x0;
    const uint32_t imm12  = static_cast<uint32_t>(imm & 0xFFF);
    return (imm12 << 20)
         | (static_cast<uint32_t>(rs1) << 15)
         | (funct3 << 12)
         | (static_cast<uint32_t>(rd) << 7)
         | opcode;
}

// I-type LOAD: lw rd, imm(rs1)  (opcode 0x03, funct3=0x2)
uint32_t encodeLW(uint8_t rd, uint8_t rs1, int32_t imm) {
    const uint32_t opcode = 0x03, funct3 = 0x2;
    const uint32_t imm12  = static_cast<uint32_t>(imm & 0xFFF);
    return (imm12 << 20)
         | (static_cast<uint32_t>(rs1) << 15)
         | (funct3 << 12)
         | (static_cast<uint32_t>(rd) << 7)
         | opcode;
}

// S-type STORE: sw rs2, imm(rs1)  (opcode 0x23, funct3=0x2)
uint32_t encodeSW(uint8_t rs2, uint8_t rs1, int32_t imm) {
    const uint32_t opcode = 0x23, funct3 = 0x2;
    const uint32_t imm12  = static_cast<uint32_t>(imm & 0xFFF);
    const uint32_t imm_low  = imm12 & 0x1F;        // bits 4..0
    const uint32_t imm_high = (imm12 >> 5) & 0x7F; // bits 11..5
    return (imm_high << 25)
         | (static_cast<uint32_t>(rs2) << 20)
         | (static_cast<uint32_t>(rs1) << 15)
         | (funct3 << 12)
         | (imm_low << 7)
         | opcode;
}

// R-type: add/sub rd, rs1, rs2 (opcode 0x33)
uint32_t encodeR(uint8_t rd, uint8_t rs1, uint8_t rs2,
                 uint8_t funct3, uint8_t funct7) {
    const uint32_t opcode = 0x33;
    return (static_cast<uint32_t>(funct7) << 25)
         | (static_cast<uint32_t>(rs2)   << 20)
         | (static_cast<uint32_t>(rs1)   << 15)
         | (static_cast<uint32_t>(funct3)<< 12)
         | (static_cast<uint32_t>(rd)    << 7)
         | opcode;
}

// B-type: BEQ/BNE (opcode 0x63). 'imm' é deslocamento em BYTES relativo ao PC.
uint32_t encodeB(uint8_t rs1, uint8_t rs2, int32_t imm, uint8_t funct3) {
    const uint32_t opcode = 0x63;

    // bit0 deve ser 0 (alinhamento de 2). Ajustamos se necessário.
    if (imm & 1) imm -= 1;

    // 13 bits com sinal (em bytes). Campos do formato B:
    // imm[12|10:5|4:1|11|0] = [31|30:25|11:8|7|0]
    const uint32_t imm13 = static_cast<uint32_t>(imm & 0x1FFF);
    const uint32_t b12   = (imm13 >> 12) & 0x1;
    const uint32_t b10_5 = (imm13 >> 5)  & 0x3F;
    const uint32_t b4_1  = (imm13 >> 1)  & 0xF;
    const uint32_t b11   = (imm13 >> 11) & 0x1;

    uint32_t inst = 0;
    inst |= (b12   << 31);
    inst |= (b10_5 << 25);
    inst |= (static_cast<uint32_t>(rs2) << 20);
    inst |= (static_cast<uint32_t>(rs1) << 15);
    inst |= (static_cast<uint32_t>(funct3 & 0x7) << 12);
    inst |= (b4_1  << 8);
    inst |= (b11   << 7);
    inst |= opcode; // 0x63

    return inst;
}

inline uint32_t encodeBEQ(uint8_t rs1, uint8_t rs2, int32_t imm) { return encodeB(rs1, rs2, imm, 0x0); }
inline uint32_t encodeBNE(uint8_t rs1, uint8_t rs2, int32_t imm) { return encodeB(rs1, rs2, imm, 0x1); }

// ====== Programa de teste ======
//
// Loop: x4 começa em 3; x5 acumula +1 a cada volta; desvia com BNE para 0x08.
// Também deixo um exemplo de LW/SW comentado abaixo.
//
int main() {
    std::cout << "RISC-V CPU Teste (ADDI/ADD/SUB/LW/SW/BEQ/BNE)\n";

    Memory mem(512);
    Bus bus(mem);
    CPU cpu(bus);

    // 0x00: addi x4, x0, 3      ; contador = 3
    // 0x04: addi x5, x0, 0      ; acumulador = 0
    // 0x08: addi x5, x5, 1      ; ++x5
    // 0x0C: addi x4, x4, -1     ; --x4
    // 0x10: bne  x4, x0, -8     ; volta para 0x08 enquanto x4 != 0
    const uint32_t inst0 = encodeADDI(4, 0, 3);
    const uint32_t inst1 = encodeADDI(5, 0, 0);
    const uint32_t inst2 = encodeADDI(5, 5, 1);
    const uint32_t inst3 = encodeADDI(4, 4, -1);
    const uint32_t inst4 = encodeBNE(4, 0, -8); // de 0x10 volta para 0x08

    mem.write32(0x00, inst0);
    mem.write32(0x04, inst1);
    mem.write32(0x08, inst2);
    mem.write32(0x0C, inst3);
    mem.write32(0x10, inst4);

    // Exemplo adicional (opcional): acesso à memória
    // 0x14: addi x10, x0, 32    ; base = 32
    // 0x18: addi x1,  x0, 123   ; x1 = 123
    // 0x1C: sw   x1,  0(x10)    ; MEM[32] = 123
    // 0x20: lw   x2,  0(x10)    ; x2 = MEM[32] = 123
    // 0x24: add  x3,  x1, x2    ; x3 = 246
    const uint32_t inst5  = encodeADDI(10, 0, 32);
    const uint32_t inst6  = encodeADDI(1,  0, 123);
    const uint32_t inst7  = encodeSW(1, 10, 0);
    const uint32_t inst8  = encodeLW(2, 10, 0);
    const uint32_t inst9  = encodeR(3, 1, 2, 0x0, 0x00); // ADD

    mem.write32(0x14, inst5);
    mem.write32(0x18, inst6);
    mem.write32(0x1C, inst7);
    mem.write32(0x20, inst8);
    mem.write32(0x24, inst9);

    std::cout << "\nDump inicial da CPU:\n";
    cpu.dump();

    std::cout << "\nExecutando 20 ciclos:\n";
    for (int i = 0; i < 20; ++i) cpu.step();

    std::cout << "\nDump final da CPU:\n";
    cpu.dump();

    return 0;
}
