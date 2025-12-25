#include "memory.h"
#include <cctype>
#include <string>
#include <stdexcept>

Memory::Memory() : cache(64, 1, 64) { reset(); }

void Memory::reset() { mem.clear(); }

bool Memory::isAddressValid(uint64_t addr, bool isWrite, bool isInstruction) const {
    if (isInstruction) return true; 
    if (addr % 8 != 0) return false; 
    if (isWrite && addr == 0xfffffffffffffff8ULL) return false; 
    return true;
}

uint8_t Memory::readByte(uint64_t addr, bool &error, bool disableCache) const {
    if (error) return 0;
    if (!disableCache) {
        cache.access(addr);
    }
    auto it = mem.find(addr);
    if (it != mem.end()) return it->second;
    return 0;
}

void Memory::writeByte(uint64_t addr, uint8_t val, bool &error) {
    if (error) return;
    mem[addr] = val;
}

uint64_t Memory::read8(uint64_t addr, bool &error, bool isInstruction, bool disableCache) const {
    if (!isAddressValid(addr, false, isInstruction)) {
        error = true;
        return 0;
    }

    if (!disableCache) {
            cache.access(addr); 
        }

    uint64_t val = 0;
    for (int i = 0; i < 8; ++i) {
        uint8_t b = readByte(addr + i, error, true);
        val |= (static_cast<uint64_t>(b) << (8 * i));
    }
    return val;
}

void Memory::write8(uint64_t addr, uint64_t val, bool &error) {
    if (!isAddressValid(addr, true, false)) {
        error = true;
        return;
    }

    cache.access(addr);

    for (int i = 0; i < 8; ++i) {
        writeByte(addr + i, static_cast<uint8_t>((val >> (8 * i)) & 0xFF), error);
    }
}

void Memory::loadYo(std::istream &in) {
    std::string line;
    while (std::getline(in, line)) {
        size_t pipePos = line.find('|');
        if (pipePos == std::string::npos) continue;
        std::string codePart = line.substr(0, pipePos);
        size_t colonPos = codePart.find(':');
        if (colonPos == std::string::npos) continue;

        std::string addrStr = codePart.substr(0, colonPos);
        std::string dataStr = codePart.substr(colonPos + 1);

        auto ltrim = [](std::string &s) {
            s.erase(0, s.find_first_not_of(" \t"));
        };
        auto rtrim = [](std::string &s) {
            s.erase(s.find_last_not_of(" \t") + 1);
        };
        ltrim(addrStr); rtrim(addrStr);
        if (addrStr.size() >= 2 && addrStr.substr(0,2) == "0x") addrStr = addrStr.substr(2);
        if (addrStr.empty()) continue;

        uint64_t addr = std::stoull(addrStr, nullptr, 16);
        for (size_t i = 0; i < dataStr.length(); ++i) {
            if (std::isspace(static_cast<unsigned char>(dataStr[i]))) continue;
            if (i + 1 < dataStr.length() && std::isxdigit(static_cast<unsigned char>(dataStr[i])) && std::isxdigit(static_cast<unsigned char>(dataStr[i+1]))) {
                std::string byteStr = dataStr.substr(i, 2);
                uint8_t byteVal = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
                mem[addr++] = byteVal;
                ++i;
            } else {
                break;
            }
        }
    }
}