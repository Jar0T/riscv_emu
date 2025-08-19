#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <Memory/IMemory.h>

struct State
{
    uint32_t program_counter = 0;
    std::array<int32_t, 32> registers = { 0 };
};


class Core
{
private:
    State state;
    std::shared_ptr<IMemory> memory;

    uint32_t fetchInstruction(uint32_t address);
    int decodeImmediateI(int32_t instruction);
    int decodeImmediateS(int32_t instruction);
    int decodeImmediateB(int32_t instruction);
    int decodeImmediateU(int32_t instruction);
    int decodeImmediateJ(int32_t instruction);

public:
    void step();

    Core(std::shared_ptr<IMemory> memory) : memory(memory) {};
    ~Core() {};
};
