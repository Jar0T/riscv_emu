#pragma once
#include <cstdint>

class IMemory {
public:
    virtual uint8_t readByte(uint32_t address) = 0;
    virtual uint16_t readHalfWord(uint32_t address) = 0;
    virtual uint32_t readWord(uint32_t address) = 0;

    virtual void writeByte(uint32_t address, uint8_t value) = 0;
    virtual void writeHalfWord(uint32_t address, uint16_t value) = 0;
    virtual void writeWord(uint32_t address, uint32_t value) = 0;
};