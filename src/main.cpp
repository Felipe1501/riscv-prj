#include <iostream>
#include "memory.hpp"
#include "bus.hpp"
#include "cpu.hpp"

int main() {
    std::cout << "Iniciando teste (cpu base)" << std::endl;

    Memory mem(64);
    Bus bus(mem);
    CPU cpu(bus);

    cpu.dump();

    std::cout << "\nSimulando 5 ciclos de clock...\n";
    for (int i = 0; i < 5; ++i) {
        cpu.step();
    }

    std::cout << "\nDump final da CPU:\n";
    cpu.dump();
    return 0;
}