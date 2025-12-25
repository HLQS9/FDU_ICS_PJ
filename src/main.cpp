#include "cpu.h"
#include <iostream>

int main() {
    CPU cpu;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    cpu.loadProgram(std::cin);

    std::cout << "[" << std::endl;

    bool isFirst = true;
    while (true) {
        cpu.step();
        cpu.printStateJSON(isFirst);
        isFirst = false;

        if (cpu.getStatus() != STAT_AOK) break;
    }

    std::cout << std::endl << "]" << std::endl;
    cpu.printCacheStats();
    return 0;
}