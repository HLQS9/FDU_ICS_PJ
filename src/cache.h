#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>

#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_RESET   "\x1b[0m"

struct CacheLine {
    bool valid = false;
    uint64_t tag = 0;
    uint64_t last_access = 0;
};

class Cache {
public:
    Cache(int S = 4, int E = 1, int B = 4); 
    void access(uint64_t addr);
    void printSummary() const;

private:
    int S, E, B;
    int s_bits, b_bits;
    uint64_t access_counter = 0;
    
    struct Set {
        std::vector<CacheLine> lines;
    };
    std::vector<Set> sets;

    long long hits = 0;
    long long misses = 0;
    long long evictions = 0;
};

#endif