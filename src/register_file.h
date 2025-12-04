#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <cstdint>
#include <array>

class RegisterFile {
public:
    RegisterFile();
    void reset();

    uint64_t read(int regId) const;
    void write(int regId, uint64_t val);
    const uint64_t* raw() const { return regs.data(); }

private:
    std::array<uint64_t, 15> regs;
};

#endif 