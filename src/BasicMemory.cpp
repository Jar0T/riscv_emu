#include <Memory/BasicMemory.h>

uint8_t BasicMemory::readByte(uint32_t address) {
    return memory.at(address);
}

uint16_t BasicMemory::readHalfWord(uint32_t address) {
    return (readByte(address + 1) << 8) | readByte(address);
}

uint32_t BasicMemory::readWord(uint32_t address) {
    return (readHalfWord(address + 2) << 16) | readHalfWord(address);
}

void BasicMemory::writeByte(uint32_t address, uint8_t value) {
    memory.at(address) = value;
}

void BasicMemory::writeHalfWord(uint32_t address, uint16_t value) {
    writeByte(address, value & 0xFF);
    writeByte(address + 1, (value >> 8) & 0xFF);
}

void BasicMemory::writeWord(uint32_t address, uint32_t value) {
    writeHalfWord(address, value & 0xFFFF);
    writeHalfWord(address + 2, (value >> 16) & 0xFFFF);
}

BasicMemory::BasicMemory(uint32_t size, std::vector<uint8_t> initialData) : size(size), memory(initialData) {
    memory.resize(size);
}
