#include "cache.h"

Cache::Cache(int S, int E, int B) : S(S), E(E), B(B) {
    s_bits = std::log2(S);
    b_bits = std::log2(B);
    sets.resize(S);
    for(auto &set : sets) set.lines.resize(E);
}

void Cache::access(uint64_t addr) {
    access_counter++;
    uint64_t setIdx = (addr >> b_bits) & (S - 1);
    uint64_t tag = addr >> (b_bits + s_bits);
    bool hit = false;
    auto &set = sets[setIdx];
    for(int i = 0; i < E; ++i) {
        if(set.lines[i].valid && set.lines[i].tag == tag) {
            hit = true;
            set.lines[i].last_access = access_counter;
            hits++;
            break;
        }
    }          
    if(!hit) {
        misses++;

        int victim = -1;
        uint64_t minTime = UINT64_MAX;
        for(int i = 0; i < E; ++i) {
            if(!set.lines[i].valid) { victim = i; break; }
        }
        if(victim == -1) {
            for(int i = 0; i < E; ++i) {
                if(set.lines[i].last_access < minTime) {
                    minTime = set.lines[i].last_access;
                    victim = i;
                }
            }
            evictions++;
        }
        set.lines[victim].valid = true;
        set.lines[victim].tag = tag;
        set.lines[victim].last_access = access_counter;
    }
}

void Cache::printSummary() const {
    std::cerr << std::dec;
    std::cerr << "=== Cache Summary ===" << std::endl;
    std::cerr << "Hits: " << hits << " | Misses: " << misses 
              << " | Rate: " << (hits+misses ? (double)hits/(hits+misses)*100 : 0) << "%" << std::endl << std::endl;
}