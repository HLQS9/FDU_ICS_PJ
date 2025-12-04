#include "cpu.h"

#include <set>
#include <iostream>
#include <iomanip>
#include <cassert>

CPU::CPU() {
    reset();
}

void CPU::reset() {
    state.reset();
    rf.reset();
    mem.reset();
    icode = ifun = 0;
    rA = rB = REG_NONE;
    valC = valP = valA = valB = valE = valM = 0;
    cnd = false;
}

void CPU::loadProgram(std::istream &in) {
    mem.loadYo(in);
    state.pc = 0;
}

uint8_t CPU::fetchByte(uint64_t addr, bool &error) const {
    return mem.readByte(addr, error);
}

Stat CPU::getStatus() const {
    return state.stat;
}

void CPU::step() {
    if (state.stat != STAT_AOK) return;

    bool memError = false;

    uint8_t byte0 = fetchByte(state.pc, memError);
    if (memError) { state.stat = STAT_ADR; return; }

    icode = (byte0 >> 4) & 0xF;
    ifun  = byte0 & 0xF;

    if (icode > I_POPQ) { state.stat = STAT_INS; return; }

    bool needReg = (icode == I_RRMOVQ || icode == I_IRMOVQ || icode == I_RMMOVQ ||
                    icode == I_MRMOVQ || icode == I_OPQ || icode == I_PUSHQ || icode == I_POPQ);

    bool needValC = (icode == I_IRMOVQ || icode == I_RMMOVQ || icode == I_MRMOVQ ||
                     icode == I_JXX || icode == I_CALL);

    uint64_t currentPC = state.pc + 1;

    if (needReg) {
        uint8_t byte1 = fetchByte(currentPC++, memError);
        if (memError) { state.stat = STAT_ADR; return; }
        rA = (byte1 >> 4) & 0xF;
        rB = byte1 & 0xF;
    } else {
        rA = REG_NONE;
        rB = REG_NONE;
    }

    if (needValC) {
        valC = mem.read8(currentPC, memError, true);
        if (memError) { state.stat = STAT_ADR; return; }
        currentPC += 8;
    } else {
        valC = 0;
    }

    valP = currentPC;

    if (icode == I_CALL || icode == I_JXX) {
        valA = valP;
    } else if (icode == I_RET || icode == I_POPQ) {
        valA = rf.read(REG_RSP);
    } else if (rA != REG_NONE) {
        valA = rf.read(rA);
    } else {
        valA = 0;
    }

    if (rB != REG_NONE) {
        valB = rf.read(rB);
    } else if (icode == I_CALL || icode == I_RET || icode == I_PUSHQ || icode == I_POPQ) {
        valB = rf.read(REG_RSP);
    } else {
        valB = 0;
    }

    valE = 0;
    cnd = false;

    switch (icode) {
        case I_HALT:
            state.stat = STAT_HLT;
            return;
        case I_NOP:
            break;
        case I_RRMOVQ:
            valE = 0 + valA;
            cnd = exec.cond(ifun, state.cc[ZF], state.cc[SF], state.cc[OF]);
            break;
        case I_IRMOVQ:
            valE = 0 + valC;
            break;
        case I_RMMOVQ:
            valE = valB + valC;
            break;
        case I_MRMOVQ:
            valE = valB + valC;
            break;
        case I_OPQ:
            valE = exec.alu(ifun, valA, valB, state.cc[ZF], state.cc[SF], state.cc[OF]);
            break;
        case I_JXX:
            cnd = exec.cond(ifun, state.cc[ZF], state.cc[SF], state.cc[OF]);
            break;
        case I_CALL:
            valE = valB - 8;
            break;
        case I_RET:
            valE = valB + 8;
            break;
        case I_PUSHQ:
            valE = valB - 8;
            break;
        case I_POPQ:
            valE = valB + 8;
            break;
        default:
            state.stat = STAT_INS;
            break;
    }
    
    bool memRead = (icode == I_MRMOVQ || icode == I_POPQ || icode == I_RET);
    bool memWrite = (icode == I_RMMOVQ || icode == I_PUSHQ || icode == I_CALL);
    uint64_t memAddr = 0;

    if (memRead) {
        memAddr = (icode == I_POPQ || icode == I_RET) ? valA : valE;
        valM = mem.read8(memAddr, memError);
        if (memError) {
            state.stat = STAT_ADR;
            return;
        }
    }

    if (memWrite) {
        memAddr = valE;
        uint64_t data = (icode == I_CALL) ? valP : valA;
        mem.write8(memAddr, data, memError);
        if (memError) {
            state.stat = STAT_ADR;
            goto write_back;
        }
    }

write_back:
    if (icode == I_OPQ || icode == I_RRMOVQ || icode == I_IRMOVQ) {
        if (icode == I_RRMOVQ && !cnd) {
        } else {
            if (rB != REG_NONE) rf.write(rB, valE);
        }
    } else if (icode == I_PUSHQ) {
        rf.write(REG_RSP, valE);
    } else if (icode == I_POPQ) {
        rf.write(REG_RSP, valE);
        if (rA != REG_NONE) rf.write(rA, valM);
    } else if (icode == I_CALL || icode == I_RET) {
        rf.write(REG_RSP, valE);
    } else if (icode == I_MRMOVQ) {
        if (rA != REG_NONE) rf.write(rA, valM);
    }
    if (state.stat == STAT_AOK) {
        if (icode == I_CALL) state.pc = valC;
        else if (icode == I_RET) state.pc = valM;
        else if (icode == I_JXX) state.pc = cnd ? valC : valP;
        else state.pc = valP;
    }
}

void CPU::printStateJSON(bool isFirst) const {
    if (!isFirst) std::cout << "," << std::endl;
    std::cout << "    {" << std::endl;
    std::cout << "        \"CC\": {" << std::endl;
    std::cout << "            \"OF\": " << (state.cc[OF] ? 1 : 0) << "," << std::endl;
    std::cout << "            \"SF\": " << (state.cc[SF] ? 1 : 0) << "," << std::endl;
    std::cout << "            \"ZF\": " << (state.cc[ZF] ? 1 : 0) << std::endl;
    std::cout << "        }," << std::endl;
    std::cout << "        \"MEM\": {";
    std::set<uint64_t> aligned;
    for (auto const &kv : mem.raw()) aligned.insert(kv.first & ~0x7ULL);
    bool firstMem = true;
    for (uint64_t base : aligned) {
        bool err = false;
        uint64_t v = const_cast<Memory&>(mem).read8(base, err);
        if (v != 0) {
            if (!firstMem) std::cout << ",";
            std::cout << std::endl << "            \"" << base << "\": " << to_signed(v);
            firstMem = false;
        }
    }
    std::cout << std::endl << "        }," << std::endl;
    std::cout << "        \"PC\": " << state.pc << "," << std::endl;
    std::cout << "        \"REG\": {" << std::endl;
    const uint64_t* rawRegs = rf.raw();
    for (int i = 0; i < 15; ++i) {
        std::cout << "            \"" << REG_NAMES[i] << "\": " << to_signed(rawRegs[i]);
        if (i < 14) std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << "        }," << std::endl;
    std::cout << "        \"STAT\": " << state.stat << std::endl;
    std::cout << "    }";
}