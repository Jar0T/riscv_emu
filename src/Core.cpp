#include <Core/Core.h>

#define LUI_OPCODE 0x37
#define AUIPC_OPCODE 0x17
#define JAL_OPCODE 0x6F
#define JALR_OPCODE 0x67
#define BRANCH_OPCODE 0x63
#define LOAD_OPCODE 0x03
#define STORE_OPCODE 0x23
#define OP_IMM_OPCODE 0x13
#define OP_REG_OPCODE 0x33
#define MISC_MEM_OPCODE 0x0F
#define SYSTEM_OPCODE 0x73

uint32_t Core::fetchInstruction(uint32_t address) {
    return memory->readWord(address);
}

int Core::decodeImmediateI(int32_t instruction) {
    return instruction >> 20;
}

int Core::decodeImmediateS(int32_t instruction) {
    return (instruction >> 25) << 5 | (instruction >> 7) & 0x1F;
}

int Core::decodeImmediateB(int32_t instruction) {
    return (instruction >> 31) << 12 |
           ((instruction >> 7) & 0x1) << 11 |
           ((instruction >> 25) & 0x3F) << 5 |
           ((instruction >> 8) & 0xF) << 1;
}

int Core::decodeImmediateU(int32_t instruction) {
    return instruction & 0xFFFFF000;
}

int Core::decodeImmediateJ(int32_t instruction) {
    return (instruction >> 31) << 20 |
           ((instruction >> 12) & 0xFF) << 12 |
           ((instruction >> 20) & 0x1) << 11 |
           ((instruction >> 21) & 0x3FF) << 1;
}

void Core::step() {
    uint32_t instruction = fetchInstruction(state.program_counter);

    uint8_t opcode = instruction & 0x7F;
    uint8_t funct3 = (instruction >> 12) & 0x07;
    uint8_t funct7 = (instruction >> 25) & 0x7F;
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;

    switch (opcode) {
    case LUI_OPCODE:
        int imm = decodeImmediateU(instruction);
        state.registers[rd] = imm;
        state.program_counter += 4;
        break;

    case AUIPC_OPCODE:
        int imm = decodeImmediateU(instruction);
        state.registers[rd] = imm + state.program_counter;
        state.program_counter += 4;
        break;

    case JAL_OPCODE:
        int imm = decodeImmediateJ(instruction);
        state.registers[rd] = state.program_counter + 4;
        state.program_counter += imm;
        break;

    case JALR_OPCODE:
        int imm = decodeImmediateI(instruction);
        state.registers[rd] = state.program_counter + 4;
        state.program_counter = (state.registers[rs1] + imm) & ~1;
        break;

    case BRANCH_OPCODE:
        int imm = decodeImmediateB(instruction);
        switch (funct3) {
        case 0x0:
            if (state.registers[rs1] == state.registers[rs2])
                state.program_counter += imm;
            else
                state.program_counter += 4;
            break;
        case 0x1:
            if (state.registers[rs1] != state.registers[rs2])
                state.program_counter += imm;
            else
                state.program_counter += 4;
            break;
        case 0x4:
            if (state.registers[rs1] < state.registers[rs2])
                state.program_counter += imm;
            else
                state.program_counter += 4;
            break;
        case 0x5:
            if (state.registers[rs1] >= state.registers[rs2])
                state.program_counter += imm;
            else
                state.program_counter += 4;
            break;
        case 0x6:
            if (static_cast<uint32_t>(state.registers[rs1]) < static_cast<uint32_t>(state.registers[rs2]))
                state.program_counter += imm;
            else
                state.program_counter += 4;
            break;
        case 0x7:
            if (static_cast<uint32_t>(state.registers[rs1]) >= static_cast<uint32_t>(state.registers[rs2]))
                state.program_counter += imm;
            else
                state.program_counter += 4;
            break;
        default:
            state.program_counter += 4;
            break;
        }
        break;

    case LOAD_OPCODE:
        int load_imm = decodeImmediateI(instruction);
        switch (funct3)
        {
        case 0x0:
            state.registers[rd] = memory->readByte(state.registers[rs1] + load_imm);
            break;

        case 0x1:
            state.registers[rd] = memory->readHalfWord(state.registers[rs1] + load_imm);
            break;

        case 0x2:
            state.registers[rd] = memory->readWord(state.registers[rs1] + load_imm);
            break;
        
        case 0x4:
            state.registers[rd] = memory->readByte(state.registers[rs1] + load_imm);
            break;

        case 0x5:
            state.registers[rd] = memory->readHalfWord(state.registers[rs1] + load_imm);
            break;

        default:
            break;
        }
        state.program_counter += 4;
        break;

    case STORE_OPCODE:
        int store_imm = decodeImmediateI(instruction);
        switch (funct3)
        {
        case 0x0:
            memory->writeByte(state.registers[rs1] + store_imm, state.registers[rs2]);
            break;

        case 0x1:
            memory->writeHalfWord(state.registers[rs1] + store_imm, state.registers[rs2]);
            break;

        case 0x2:
            memory->writeWord(state.registers[rs1] + store_imm, state.registers[rs2]);
            break;

        default:
            break;
        }
        state.program_counter += 4;
        break;

    case OP_IMM_OPCODE:
        int imm = decodeImmediateI(instruction);
        switch (funct3) {
        case 0x0:
            state.registers[rd] = state.registers[rs1] + imm;
            break;
        case 0x1:
            state.registers[rd] = state.registers[rs1] << imm;
            break;
        case 0x2:
            state.registers[rd] = state.registers[rs1] < imm ? 1 : 0;
            break;
        case 0x3:
            state.registers[rd] = static_cast<uint32_t>(state.registers[rs1]) < static_cast<uint32_t>(imm) ? 1 : 0;
            break;
        case 0x4:
            state.registers[rd] = state.registers[rs1] ^ imm;
            break;
        case 0x5:
            if (funct7 == 0x00) {
                state.registers[rd] = static_cast<uint32_t>(state.registers[rs1]) >> imm;
            } else {
                state.registers[rd] = state.registers[rs1] >> imm;
            }
            break;
        case 0x6:
            state.registers[rd] = state.registers[rs1] | imm;
            break;
        case 0x7:
            state.registers[rd] = state.registers[rs1] & imm;
            break;
        default:
            break;
        }
        state.program_counter += 4;
        break;

    case OP_REG_OPCODE:
        switch (funct3) {
        case 0x0:
            if (funct7 == 0x00) {
                state.registers[rd] = state.registers[rs1] + state.registers[rs2];
            } else {
                state.registers[rd] = state.registers[rs1] - state.registers[rs2];
            }
            break;
        case 0x1:
            state.registers[rd] = state.registers[rs1] << state.registers[rs2];
            break;
        case 0x2:
            state.registers[rd] = state.registers[rs1] < state.registers[rs2] ? 1 : 0;
            break;
        case 0x3:
            state.registers[rd] = static_cast<uint32_t>(state.registers[rs1]) < static_cast<uint32_t>(state.registers[rs2]) ? 1 : 0;
            break;
        case 0x4:
            state.registers[rd] = state.registers[rs1] ^ state.registers[rs2];
            break;
        case 0x5:
            if (funct7 == 0x00) {
                state.registers[rd] = static_cast<uint32_t>(state.registers[rs1]) >> state.registers[rs2];
            } else {
                state.registers[rd] = state.registers[rs1] >> state.registers[rs2];
            }
            break;
        case 0x6:
            state.registers[rd] = state.registers[rs1] | state.registers[rs2];
            break;
        case 0x7:
            state.registers[rd] = state.registers[rs1] & state.registers[rs2];
            break;
        default:
            break;
        }
        state.program_counter += 4;
        break;

    case MISC_MEM_OPCODE:
        state.program_counter += 4;
        break;

    case SYSTEM_OPCODE:
        state.program_counter += 4;
        break;

    default:
        break;
    }
}