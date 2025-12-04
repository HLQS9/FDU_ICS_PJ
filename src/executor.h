#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <cstdint>

class Executor {
public:
    uint64_t alu(uint8_t ifun, uint64_t a, uint64_t b, bool &ZF, bool &SF, bool &OF);
    bool cond(uint8_t func, bool ZF, bool SF, bool OF) const;
};

#endif