#ifndef CPU_H
#define CPU_H

#include "state.h"
#include "register_file.h"
#include "memory.h"
#include "executor.h"
#include "isa.h"

#include <iostream>

class CPU {
public:
    CPU();
    void reset();
    void loadProgram(std::istream &in);
    void step();
    Stat getStatus() const;
    void printStateJSON(bool isFirst) const;
private:
    uint8_t fetchByte(uint64_t addr, bool &error) const;

    uint8_t icode, ifun;
    uint8_t rA, rB;
    uint64_t valC;
    uint64_t valP;
    uint64_t valA, valB;
    uint64_t valE, valM;
    bool cnd;

    CPUState state;
    RegisterFile rf;
    Memory mem;
    Executor exec;
};

#endif 