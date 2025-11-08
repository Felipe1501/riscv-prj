#include "cpu.hpp"

CPU::CPU(Bus& b) 
    : bus(b) {
    reset();
}

void CPU::reset() {
    pc = 0;
    x.fill(0);
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

void CPU::step(){
    std::cout << "[CPU] Executando ciclo no @PC = " << pc << std::endl;
    pc += 4;
}