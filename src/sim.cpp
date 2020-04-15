#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <bitset>
#include "MemoryStore.h"
#include "RegisterInfo.h"
#include "EndianHelpers.h"

#define NUM_REGS 32
#define EXIT_FAILURE 1
#define EXIT_INSTRUCTION 0xfeedfeed

#define SIGNEX(v, sb) ((v) | (((v) & (1 << (sb))) ? ~((1 << (sb))-1) : 0))

using namespace std;

//-------------------------------------------------------------------------
// Offsets of the various registers in the 'regs' array
//-------------------------------------------------------------------------
enum RegOffsets {
    AT_OFFSET  = 1,
    V_OFFSET   = 2,
    A_OFFSET   = 4,
    T07_OFFSET = 8,
    S_OFFSET   = 16,
    T89_OFFSET = 24,
    T8_OFFSET  = 24,
    T9_OFFSET  = 25,
    K_OFFSET   = 26,
    GP_OFFSET  = 28,
    SP_OFFSET  = 29, 
    FP_OFFSET  = 30,
    RA_OFFSET  = 31
};

//-------------------------------------------------------------------------
// Masks and shift sizes used to slice fields from a 32-bit value
//-------------------------------------------------------------------------
enum Slicing {
    // Masks used to isolate a certain field of an instruction
    OPCODE_MASK = 0xFC000000, // 11111100000000000000000000000000
    RS_MASK     = 0x03E00000, // 00000011111000000000000000000000
    RT_MASK     = 0x001F0000, // 00000000000111110000000000000000
    RD_MASK     = 0x0000F800, // 00000000000000001111100000000000
    SHAMT_MASK  = 0x000007C0, // 00000000000000000000011111000000
    FUNCT_MASK  = 0x0000003F, // 00000000000000000000000000111111
    IMM_MASK    = 0x0000FFFF, // 00000000000000001111111111111111
    ADDR_MASK   = 0x03FFFFFF, // 00000011111111111111111111111111

    // Number of bits to right shift in order to obtain the true value of the field
    OPCODE_BITS = 26,
    RS_BITS     = 21,
    RT_BITS     = 16,
    RD_BITS     = 11,
    SHAMT_BITS  = 6,
    FUNCT_BITS  = 0,
    IMM_BITS    = 0,
    ADDR_BITS   = 0,

    // Sign-bit offset used for sign extending
    IMM_SIGN_BIT = 15,

    // Masks used for isolating byte/word in load/store
    BYTE_MASK = 0x000000FF, // 00000000000000000000000011111111
    HALF_MASK = 0x0000FFFF, // 00000000000000001111111111111111

    // Mask used to isolate PC[31:28] for JumpAddr calculation
    PC_MASK = 0xF000000 // 11110000000000000000000000000000
};

//-------------------------------------------------------------------------
// Opcodes and Functs of the various operations to be implemented
//-------------------------------------------------------------------------
enum Instr_OpFunct {
    // Instruction Opcodes
    ADDI_O  = 0x8,
    ADDIU_O = 0x9,
    ANDI_O  = 0xc,
    BEQ_O   = 0x4,
    BNE_O   = 0x5,
    BLEZ_O  = 0x6,
    BGTZ_O  = 0x7,
    J_O     = 0x2,
    JAL_O   = 0x3,
    LBU_O   = 0x24,
    LHU_O   = 0x25,
    LUI_O   = 0xf,
    LW_O    = 0x23,
    ORI_O   = 0xd,
    SLTI_O  = 0xa,
    SLTIU_O = 0xb,
    SB_O    = 0x28,
    SH_O    = 0x29,
    SW_O    = 0x2b,

    // Instruction Functs (For when instruction opcode == 0x00)
    ADD_F  = 0x20,
    ADDU_F = 0x21,
    AND_F  = 0x24,
    JR_F   = 0x8,
    NOR_F  = 0x27,
    OR_F   = 0x25,
    SLT_F  = 0x2a,
    SLTU_F = 0x2b,
    SLL_F  = 0x0,
    SRL_F  = 0x2,
    SUB_F  = 0x22,
    SUBU_F = 0x23
};

//-------------------------------------------------------------------------
// Helper function which loads the RegisterInfo struct then dumps regs/mem
//-------------------------------------------------------------------------
void dump(uint32_t (&regs)[NUM_REGS], MemoryStore *myMem) {
    RegisterInfo reg;
    for (int i = 1; i < NUM_REGS; i++) {
        switch (i) {
            case AT_OFFSET:                         // regs[1] - The $at register.
                reg.at = regs[i];
                break;
            case V_OFFSET ... A_OFFSET-1:           // regs[2:3] - The $v registers.
                reg.v[i - V_OFFSET] = regs[i];
                break;
            case A_OFFSET ... T07_OFFSET-1:         // regs[4:7] - The $a registers.
                reg.a[i - A_OFFSET] = regs[i];
                break;
            case T07_OFFSET ... S_OFFSET-1:         // regs[8:15] - The $t0-$t7 registers.
                reg.t[i - T07_OFFSET] = regs[i];
                break;
            case S_OFFSET ... T8_OFFSET-1:          // regs[16:23] - The $s registers.
                reg.s[i - S_OFFSET] = regs[i];
                break;
            case T8_OFFSET:                         // regs[24] - The $t8 register.
                reg.t[8] = regs[i];
                break;
            case T9_OFFSET:                         // regs[25] - The $t9 register.
                reg.t[9] = regs[i];
                break;
            case K_OFFSET ... GP_OFFSET-1:          // regs[26:27] - The $k registers.
                reg.k[i - K_OFFSET] = regs[i];
                break;
            case GP_OFFSET:                         // regs[28] - The $gp register.
                reg.gp = regs[i];
                break;
            case SP_OFFSET:                         // regs[29] - The $sp register.
                reg.sp = regs[i];
                break;
            case FP_OFFSET:                         // regs[30] - The $fp register.
                reg.fp = regs[i];
                break;
            case RA_OFFSET:                         // regs[31] - The $ra register.
                reg.ra = regs[i];
                break;
        }
    }
    dumpRegisterState(reg);
    dumpMemoryState(myMem);
}

//-------------------------------------------------------------------------
// Helper function which does the fetch/decode/execute steps
//-------------------------------------------------------------------------
void instruction_helper(uint32_t &pc, uint32_t (&regs)[NUM_REGS], MemoryStore *myMem) {
    // Fetch instruction
    uint32_t instr;
    myMem->getMemValue(pc, instr, WORD_SIZE);

    // Increment PC immediately. Operations below will be on the incremented PC and will take this into account
    pc = pc + 4;

    // Decode instruction by isolating fields
    uint32_t opcode = (instr & OPCODE_MASK) >> OPCODE_BITS;
    uint32_t rs     = (instr & RS_MASK) >> RS_BITS;
    uint32_t rt     = (instr & RT_MASK) >> RT_BITS;
    uint32_t rd     = (instr & RD_MASK) >> RD_BITS;
    uint32_t shamt  = (instr & SHAMT_MASK) >> SHAMT_BITS;
    uint32_t funct  = (instr & FUNCT_MASK) >> FUNCT_BITS;
    uint32_t imm    = (instr & IMM_MASK) >> IMM_BITS;
    uint32_t addr   = (instr & ADDR_MASK) >> ADDR_BITS;

    // Exit Case
    if (instr == EXIT_INSTRUCTION) {
        dump(regs, myMem);
        exit(0);
    }

    /* Important notes regarding some implementation details below
     *
     *  1. add/addu, addi/addiu, sub/subu  only differ in that the signed functions are expected to
     *     catch and handle overflow. The C operations happening under the hood are the same.
     *     Since P1 doesn't require such error handling, the implementations below are identical
     * 
     *  2. All control flow instructions (j, jal, jr, beq, bne, blez, bgtz) simulate delay slots
     *     by recursively calling instruction_helper() on the next instruction (pointed to by
     *     pc, which has already been incremented above). Within the recursive call, the already-
     *     incremented PC is incremented once more. So by the time the delay slot simulation finishes,
     *     pc will be pointing to the next next instruction (what the green card would refer to as PC+8).
     *     This is why you may see me use (pc - 4) where you might expect to see PC+4, or pc where you
     *     might expect to see PC+8.
     * 
     *  3. While this function is called recursively for delay slot simulations, there is no exit case
     *     for the recursion. So make sure not to have a control flow instruction following another
     *     control flow instruction, otherwise you'll end up in an infinite loop.
     * 
     *  4. In slti, operands are supposed to be treated as 2's comp ints. Because my variables are 
     *     unsigned, I cast them to (int) so that the < operator treats them as signed.
     */
    switch (opcode) {
        case 0x00:
            switch (funct) {
                case ADD_F:
                    regs[rd] = regs[rs] + regs[rt];
                    break;

                case ADDU_F:
                    regs[rd] = regs[rs] + regs[rt];
                    break;

                case AND_F:
                    regs[rd] = regs[rs] & regs[rt];
                    break;

                case JR_F:
                    instruction_helper(pc, regs, myMem);
                    pc = regs[rs];
                    break;

                case NOR_F:
                    regs[rd] = ~ (regs[rs] | regs[rt]);
                    break;

                case OR_F:
                    regs[rd] = regs[rs] | regs[rt];
                    break;

                case SLT_F:
                    regs[rd] = ((int) regs[rs] < (int) regs[rt]) ? 1 : 0;
                    break;
                    
                case SLTU_F:
                    regs[rd] = (regs[rs] < regs[rt]) ? 1 : 0;
                    break;

                case SLL_F:
                    regs[rd] = regs[rt] << shamt;
                    break;
                    
                case SRL_F:
                    regs[rd] = regs[rt] >> shamt;
                    break;

                case SUB_F:
                    regs[rd] = regs[rs] - regs[rt];
                    break;

                case SUBU_F:
                    regs[rd] = regs[rs] - regs[rt];
                    break;
                    
                default:
                    printf("Illegal operation...\n");
                    exit(127);
            }
            break;

        case ADDI_O:
            regs[rt] = regs[rs] + SIGNEX(imm, IMM_SIGN_BIT);
            break;

        case ADDIU_O:
            regs[rt] = regs[rs] + SIGNEX(imm, IMM_SIGN_BIT);
            break;

        case ANDI_O:
            regs[rt] = regs[rs] & imm; // NOTE: imm is already zero-extended by default
            break;

        case BEQ_O:
            instruction_helper(pc, regs, myMem);
            if ((int) regs[rs] == (int) regs[rt]) { // NOTE: int casts don't do anything since it's a bitwise comp. just inserted for continuity
                pc = (pc - 4) + SIGNEX(imm << 2, 17);
            }
            break;

        case BNE_O:
            instruction_helper(pc, regs, myMem);
            if ((int) regs[rs] != (int) regs[rt]) { // NOTE: int casts don't do anything since it's a bitwise comp. just inserted for continuity
                pc = (pc - 4) + SIGNEX(imm << 2, 17);;
            }
            break;
        
        case BGTZ_O:
            instruction_helper(pc, regs, myMem);
            if ((int) regs[rs] > 0) {
                pc = (pc - 4) + SIGNEX(imm << 2, 17);
            }
            break;

        case BLEZ_O:
            instruction_helper(pc, regs, myMem);
            if ((int) regs[rs] <= 0) {
                pc = (pc - 4) + SIGNEX(imm << 2, 17);;
            }
            break;

        case J_O:
            instruction_helper(pc, regs, myMem);
            pc = ((pc - 4) & PC_MASK) | (addr << 2);
            break;

        case JAL_O:
            instruction_helper(pc, regs, myMem);
            regs[RA_OFFSET] = pc;
            pc = ((pc - 4) & PC_MASK) | (addr << 2);
            break;

        case LBU_O:
            myMem->getMemValue(regs[rs]+SIGNEX(imm, IMM_SIGN_BIT), regs[rt], BYTE_SIZE);
            break;
        
        case LHU_O:
            myMem->getMemValue(regs[rs]+SIGNEX(imm, IMM_SIGN_BIT), regs[rt], HALF_SIZE);
            regs[rt] = HALF_MASK & regs[rt];
            break;

        case LUI_O:
            regs[rt] = imm << 16;
            break;

        case LW_O:
            myMem->getMemValue(regs[rs]+SIGNEX(imm, IMM_SIGN_BIT), regs[rt], WORD_SIZE);
            break;

        case ORI_O:
            regs[rt] = regs[rs] | imm; // NOTE: imm is already zero-extended by default
            break;

        case SLTI_O:
            regs[rt] = ((int) regs[rs] < (int) SIGNEX(imm, IMM_SIGN_BIT)) ? 1 : 0;
            break;

        case SLTIU_O:
            regs[rt] = ((uint32_t) regs[rs] < (uint32_t) SIGNEX(imm, IMM_SIGN_BIT)) ? 1 : 0;
            break;

        case SB_O:
            myMem->setMemValue(regs[rs] + SIGNEX(imm, IMM_SIGN_BIT), BYTE_MASK & regs[rt], BYTE_SIZE);
            break;

        case SH_O:
            myMem->setMemValue(regs[rs] + SIGNEX(imm, IMM_SIGN_BIT), HALF_MASK & regs[rt], HALF_SIZE);
            break;

        case SW_O:
            myMem->setMemValue(regs[rs] + SIGNEX(imm, IMM_SIGN_BIT), regs[rt], WORD_SIZE);
            break;

        default:
            printf("Illegal operation...\n");
            exit(127);
    }
}

int main(int argc, char **argv) {
    // Create MemoryStore object, Create registers (init to 0), Create PC
    MemoryStore *myMem = createMemoryStore();
    uint32_t regs[NUM_REGS] = {0};

    // Point PC to address 0 of MemoryStore object (where first instr will be)
    uint32_t pc = 0;

    // Parse arguments to obtain name of binary file
    if (argc != 2) {
        cout << "Invalid number of arguments";
        dump(regs, myMem);
        exit(EXIT_FAILURE);
    }
    char *filename = argv[1];

    // Parse binary file and copy data into MemoryStore object
    streampos size;
    char * memblock;
    ifstream file (filename, ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        // read entire file into real memory
        size = file.tellg(); // size in bytes
        memblock = new char[size];
        file.seekg (0, ios::beg);
        file.read (memblock, size);
        file.close();

        // Printing parsed binary file
        printf("\nFile Size (Bytes): %#x\n\n", (int)size);
        for (int i = 0; i < size; i++) {
            printf("%02hhx", memblock[i]);
            if ((i+1) % 4 == 0) printf(" ");
            if ((i+1) % 20 == 0) printf("\n");
        }
        printf("\n\n");

        // transfer file line-by-line from real memory to abstracted MemoryStore memory
        if (size % WORD_SIZE != 0) {
            cout << "Binary file should have an integer number of 4-byte instructions";
            exit(EXIT_FAILURE);
        }
        for (uint32_t i = 0; i <= size; i++) {
            myMem->setMemValue(i, (uint32_t) memblock[i], BYTE_SIZE);
        }
        // delete initial allocation of real memory. Now use MemoryStore object
        delete[] memblock;
    }
    else {
        printf("Unable to open file %s\n", filename);
        dump(regs, myMem);
        exit(EXIT_FAILURE);
    }

    // Fetch/Decode/Execute instructions, exit upon encountering instruction 0xfeedfeed
    while (true) {
        instruction_helper(pc, regs, myMem);
    }
}