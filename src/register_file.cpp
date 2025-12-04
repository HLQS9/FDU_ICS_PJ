#include "register_file.h"
#include "isa.h"
#include <cassert>

RegisterFile::RegisterFile() {
    reset();
}

void RegisterFile::reset() {
    for (auto &r : regs) r = 0;
}

uint64_t RegisterFile::read(int regId) const {
    if (regId < 0 || regId >= 15) return 0;
    return regs[regId];
}

void RegisterFile::write(int regId, uint64_t val) {
    if (regId < 0 || regId >= 15) return;
    regs[regId] = val;
}