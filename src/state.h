#ifndef STATE_H
#define STATE_H

#include "isa.h"

struct CPUState {
    uint64_t pc;
    bool cc[3]; // ZF, SF, OF
    Stat stat;

    CPUState() {
        reset();
    }

    void reset() {
        pc = 0;
        cc[ZF] = true;
        cc[SF] = false;
        cc[OF] = false;
        stat = STAT_AOK;
    }
};

#endif