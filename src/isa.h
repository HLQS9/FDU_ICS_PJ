#ifndef ISA_H
#define ISA_H

#include <cstdint>
#include <string>

enum Stat {
    STAT_AOK = 1,
    STAT_HLT = 2,
    STAT_ADR = 3,
    STAT_INS = 4
};

enum CC_Flag {
    ZF = 0,
    SF = 1,
    OF = 2
};

inline const int REG_RAX = 0;
inline const int REG_RCX = 1;
inline const int REG_RDX = 2;
inline const int REG_RBX = 3;
inline const int REG_RSP = 4;
inline const int REG_RBP = 5;
inline const int REG_RSI = 6;
inline const int REG_RDI = 7;
inline const int REG_R8  = 8;
inline const int REG_R9  = 9;
inline const int REG_R10 = 10;
inline const int REG_R11 = 11;
inline const int REG_R12 = 12;
inline const int REG_R13 = 13;
inline const int REG_R14 = 14;
inline const int REG_NONE= 0xF;

inline const std::string REG_NAMES[] = {
    "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
    "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "none"
};

inline const uint8_t I_HALT   = 0x0;
inline const uint8_t I_NOP    = 0x1;
inline const uint8_t I_RRMOVQ = 0x2;
inline const uint8_t I_IRMOVQ = 0x3;
inline const uint8_t I_RMMOVQ = 0x4;
inline const uint8_t I_MRMOVQ = 0x5;
inline const uint8_t I_OPQ    = 0x6;
inline const uint8_t I_JXX    = 0x7;
inline const uint8_t I_CALL   = 0x8;
inline const uint8_t I_RET    = 0x9;
inline const uint8_t I_PUSHQ  = 0xA;
inline const uint8_t I_POPQ   = 0xB;

inline const uint8_t F_ADD = 0;
inline const uint8_t F_SUB = 1;
inline const uint8_t F_AND = 2;
inline const uint8_t F_XOR = 3;

inline const uint8_t C_NC = 0;
inline const uint8_t C_LE = 1;
inline const uint8_t C_L  = 2;
inline const uint8_t C_E  = 3;
inline const uint8_t C_NE = 4;
inline const uint8_t C_GE = 5;
inline const uint8_t C_G  = 6;

inline int64_t to_signed(uint64_t v) { return static_cast<int64_t>(v); }

#endif