#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <map>
#include <istream>

class Memory {
public:
    Memory();
    void reset();

    uint8_t readByte(uint64_t addr, bool &error) const;
    void writeByte(uint64_t addr, uint8_t val, bool &error);
    uint64_t read8(uint64_t addr, bool &error, bool isInstruction = false) const;
    void write8(uint64_t addr, uint64_t val, bool &error);
    void loadYo(std::istream &in);
    std::map<uint64_t,uint8_t> const& raw() const { return mem; }

private:
    bool isAddressValid(uint64_t addr, bool isWrite=false, bool isInstruction=false) const;

    std::map<uint64_t,uint8_t> mem;
};

#endif 