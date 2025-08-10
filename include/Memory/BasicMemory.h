#pragma once
#include <vector>
#include <cstdint>
#include <Memory/IMemory.h>

class BasicMemory : public IMemory {
private:
    std::vector<uint8_t> memory;
    uint32_t size;

public:
    uint8_t readByte(uint32_t address);
    uint16_t readHalfWord(uint32_t address);
    uint32_t readWord(uint32_t address);
    
    void writeByte(uint32_t address, uint8_t value);
    void writeHalfWord(uint32_t address, uint16_t value);
    void writeWord(uint32_t address, uint32_t value);

    BasicMemory(uint32_t size, std::vector<uint8_t> initialData);
    ~BasicMemory() {};
};
