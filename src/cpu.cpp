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
    for (int i = 0; i < 8; i++) {
        std::cout << "x" << i << " = " << x[i] << "\t";
    }
    std::cout << std::endl;
}

uint32_t CPU::getBits(uint32_t value, int hi, int lo) const {
    const uint32_t mask = ((1u << (hi - lo + 1)) - 1u);
    return (value >> lo) & mask;
}

int32_t CPU::signExtend(uint32_t value, int bits) const {
    int32_t s = static_cast<int32_t>(value);
    int shift = 32 - bits;
    return (s << shift) >> shift;
}

void CPU::step() {
    std::cout << "\n[CPU] Executando ciclo no @PC = " << pc << std::endl;

    // 1) BUSCA
    uint32_t inst = bus.read32(pc);
    std::cout << "[CPU] Instrucao lida: 0x" << std::hex << inst << std::dec << std::endl;

    // 2) DECODE – campos comuns
    uint32_t opcode = getBits(inst, 6, 0);
    uint32_t rd     = getBits(inst, 11, 7);
    uint32_t funct3 = getBits(inst, 14, 12);
    uint32_t rs1    = getBits(inst, 19, 15);
    uint32_t rs2    = getBits(inst, 24, 20);
    uint32_t funct7 = getBits(inst, 31, 25);

    std::cout << "[CPU] opcode=" << opcode
              << ", rd=" << rd
              << ", funct3=" << funct3
              << ", rs1=" << rs1
              << ", rs2=" << rs2
              << ", funct7=" << funct7 << std::endl;

    uint32_t next_pc = pc + 4;  // padrão: segue para próxima instrução

    // 3) EXECUTE
    if (opcode == 0x13) {
        // I-type aritmético => ADDI
        uint32_t imm12 = getBits(inst, 31, 20);
        int32_t imm = signExtend(imm12, 12);
        std::cout << "[CPU] (I-type) imm=" << imm << std::endl;

        if (funct3 == 0x0) { // ADDI
            uint32_t old = x[rs1];
            uint32_t result = old + imm;
            if (rd != 0) x[rd] = result;
            std::cout << "[CPU] Executando ADDI: x" << rd
                      << " = x" << rs1 << "(" << old << ") + "
                      << imm << " -> " << result << std::endl;
        } else {
            std::cout << "[CPU] I-type nao implementado.\n";
        }

    } else if (opcode == 0x33) {
        // R-type: ADD, SUB
        if (funct3 == 0x0 && funct7 == 0x00) { // ADD
            uint32_t a = x[rs1], b = x[rs2];
            uint32_t r = a + b;
            if (rd != 0) x[rd] = r;
            std::cout << "[CPU] Executando ADD: x" << rd
                      << " = x" << rs1 << "(" << a << ") + x" << rs2
                      << "(" << b << ") -> " << r << std::endl;
        } else if (funct3 == 0x0 && funct7 == 0x20) { // SUB
            uint32_t a = x[rs1], b = x[rs2];
            uint32_t r = a - b;
            if (rd != 0) x[rd] = r;
            std::cout << "[CPU] Executando SUB: x" << rd
                      << " = x" << rs1 << "(" << a << ") - x" << rs2
                      << "(" << b << ") -> " << r << std::endl;
        } else {
            std::cout << "[CPU] R-type nao implementado.\n";
        }

    } else if (opcode == 0x03) {
        // LOADS (I-type) -> LW
        uint32_t imm12 = getBits(inst, 31, 20);
        int32_t imm = signExtend(imm12, 12);
        uint32_t addr = x[rs1] + imm;

        if (funct3 == 0x2) { // LW
            if (addr % 4 != 0) {
                std::cout << "[CPU] EXC: LW desalinhado em " << addr << " (ignorado)\n";
            } else {
                uint32_t val = bus.read32(addr);
                if (rd != 0) x[rd] = val;
                std::cout << "[CPU] Executando LW: x" << rd
                          << " <- MEM[" << addr << "] = " << val << std::endl;
            }
        } else {
            std::cout << "[CPU] LOAD nao implementado.\n";
        }

    } else if (opcode == 0x23) {
        // STORES (S-type) -> SW
        uint32_t imm_high = getBits(inst, 31, 25);
        uint32_t imm_low  = getBits(inst, 11, 7);
        uint32_t imm12u   = (imm_high << 5) | imm_low;
        int32_t imm = signExtend(imm12u, 12);
        uint32_t addr = x[rs1] + imm;

        if (funct3 == 0x2) { // SW
            if (addr % 4 != 0) {
                std::cout << "[CPU] EXC: SW desalinhado em " << addr << " (ignorado)\n";
            } else {
                uint32_t val = x[rs2];
                bus.write32(addr, val);
                std::cout << "[CPU] Executando SW: MEM[" << addr
                          << "] <- x" << rs2 << "(" << val << ")\n";
            }
        } else {
            std::cout << "[CPU] STORE nao implementado.\n";
        }

    } else if (opcode == 0x63) {
        // BRANCHES (B-type): BEQ/BNE
        // imm[12|10:5|4:1|11|0] = [31|30:25|11:8|7|0]
        uint32_t imm12 =  (getBits(inst, 31, 31) << 12)
                        | (getBits(inst, 30, 25) << 5)
                        | (getBits(inst, 11,  8) << 1)
                        | (getBits(inst,  7,  7) << 11);
        int32_t imm = signExtend(imm12, 13);
        uint32_t target = pc + imm;

        bool taken = false;
        if (funct3 == 0x0) {            // BEQ
            taken = (x[rs1] == x[rs2]);
            std::cout << "[CPU] BEQ " << (taken ? "TOMADO" : "NAO tomado")
                      << " (x" << rs1 << "=" << x[rs1]
                      << ", x" << rs2 << "=" << x[rs2]
                      << "), target=" << target << "\n";
        } else if (funct3 == 0x1) {     // BNE
            taken = (x[rs1] != x[rs2]);
            std::cout << "[CPU] BNE " << (taken ? "TOMADO" : "NAO tomado")
                      << " (x" << rs1 << "=" << x[rs1]
                      << ", x" << rs2 << "=" << x[rs2]
                      << "), target=" << target << "\n";
        } else {
            std::cout << "[CPU] BRANCH nao implementado.\n";
        }

        if (taken) next_pc = target;

    } else {
        std::cout << "[CPU] Instrucao desconhecida. NOP.\n";
    }

    // 4) Atualiza PC conforme decidido
    pc = next_pc;

    // 5) Garante x0 = 0
    x[0] = 0;
}
