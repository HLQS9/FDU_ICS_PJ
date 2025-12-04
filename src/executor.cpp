#include "executor.h"
#include "isa.h"
#include <cstdint>

uint64_t Executor::alu(uint8_t ifun, uint64_t a, uint64_t b, bool &ZF, bool &SF, bool &OF) {
    int64_t argA = static_cast<int64_t>(a);
    int64_t argB = static_cast<int64_t>(b);
    int64_t res = 0;

    if (ifun == F_ADD) res = argB + argA;
    else if (ifun == F_SUB) res = argB - argA; 
    else if (ifun == F_AND) res = argB & argA;
    else if (ifun == F_XOR) res = argB ^ argA;
    else res = 0;

    ZF = (res == 0);
    SF = (res < 0);

    if (ifun == F_ADD) {
        OF = ((argA < 0) == (argB < 0)) && ((res < 0) != (argA < 0));
    } else if (ifun == F_SUB) {
        OF = ((argA < 0) != (argB < 0)) && ((res < 0) != (argB < 0));
    } else {
        OF = false;
    }

    return static_cast<uint64_t>(res);
}

bool Executor::cond(uint8_t func, bool ZF, bool SF, bool OF) const {
    switch(func) {
        case C_NC: return true;
        case C_LE: return (SF ^ OF) | ZF;
        case C_L:  return (SF ^ OF);
        case C_E:  return ZF;
        case C_NE: return !ZF;
        case C_GE: return !(SF ^ OF);
        case C_G:  return (!(SF ^ OF)) & (!ZF);
        default: return false;
    }
}