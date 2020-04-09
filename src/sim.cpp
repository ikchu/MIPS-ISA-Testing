#include <iostream>
#include <fstream>
#include <iomanip>
#include "MemoryStore.h"
#include "RegisterInfo.h"
#include "EndianHelpers.h"

#define NUM_REGS 32

//-------------------------------------------------------------------------
// Offset of the given register groupings in my 'regs' array
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
// Masks and shift sizes used to slice fields from a 32-bit instruction
//-------------------------------------------------------------------------
enum Slicing {
    // Masks used to isolate a certain field of an instruction
    OPCODE_MASK = 0xFC000000, // 11111100000000000000000000000000
    RS_MASK     = 0x03E00000, // 00000011111000000000000000000000
    RT_MASK     = 0x001F0000, // 00000000000111110000000000000000
    RD_MASK     = 0x0000F800, // 00000000000000001111100000000000
    FUNCT_MASK  = 0x0000003F, // 00000000000000000000000000111111
    IMM_MASK    = 0x0000FFFF, // 00000000000000001111111111111111
    ADDR_MASK   = 0x03FFFFFF, // 00000011111111111111111111111111

    // Number of bits to right shift in order to obtain the true value of the field
    OPCODE_BITS = 26,
    RS_BITS     = 21,
    RT_BITS     = 16,
    RD_BITS     = 11,
    FUNCT_BITS  = 0,
    IMM_BITS    = 0,
    ADDR_BITS   = 0
};

using namespace std;

void dump(uint32_t (&regs)[NUM_REGS], MemoryStore *myMem) {
// void dump(uint32_t *regs, MemoryStore *myMem) {
    RegisterInfo reg;
    for (int i = 1; i < NUM_REGS; i++) {
        switch (i) {
            case AT_OFFSET:                         // regs[1] - The $at register.
                reg.at = regs[i];
            case V_OFFSET ... A_OFFSET-1:           // regs[2:3] - The $v registers.
                reg.v[i - V_OFFSET] = regs[i];
            case A_OFFSET ... T07_OFFSET-1:         // regs[4:7] - The $a registers.
                reg.a[i - A_OFFSET] = regs[i];
            case T07_OFFSET ... S_OFFSET-1:         // regs[8:15] - The $t0-$t7 registers.
                reg.t[i - T07_OFFSET] = regs[i];
            case S_OFFSET ... T8_OFFSET-1:          // regs[16:23] - The $s registers.
                reg.s[i - S_OFFSET] = regs[i];
            case T8_OFFSET:                         // regs[24] - The $t8 register.
                reg.t[8] = regs[i];
            case T9_OFFSET:                         // regs[25] - The $t9 register.
                reg.t[9] = regs[i];
            case K_OFFSET ... GP_OFFSET-1:          // regs[26:27] - The $k registers.
                reg.k[i - K_OFFSET] = regs[i];
            case GP_OFFSET:                         // regs[28] - The $gp register.
                reg.gp = regs[i];
            case SP_OFFSET:                         // regs[29] - The $sp register.
                reg.sp = regs[i];
            case FP_OFFSET:                         // regs[30] - The $fp register.
                reg.fp = regs[i];
            case RA_OFFSET:                         // regs[31] - The $ra register.
                reg.ra = regs[i];
        }
    }
    dumpRegisterState(reg);
    dumpMemoryState(myMem);
}

int main(int argc, char **argv) {
    //-------------------------------------------------------------------------
    // Create MemoryStore object, Create registers (init to 0), Create PC
    // Point PC to address 0 of MemoryStore object (where first instr will be)
    //-------------------------------------------------------------------------
    MemoryStore *myMem = createMemoryStore();
    uint32_t regs[NUM_REGS] = {0};
    uint16_t pc = 0;

    //-------------------------------------------------------------------------
    // Parse arguments to obtain name of binary file
    //-------------------------------------------------------------------------
    if (argc != 2) {
        cout << "Invalid number of arguments";
        dump(regs, myMem);
        return 1;
    }
    char *filename = argv[1];

    //-------------------------------------------------------------------------
    // Parse binary file and copy data into MemoryStore object
    //-------------------------------------------------------------------------
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

        printf("\nFile Size (Bytes): %#x\n\n", (int)size);
        for (int i = 0; i < size; i++) {
            printf("%02hhx", memblock[i]);
            if ((i+1) % 4 == 0) printf(" ");
            if ((i+1) % 20 == 0) printf("\n");
        }

        // transfer file line-by-line from real memory to abstract MemoryStore memory
        if (size % WORD_SIZE != 0) { // maybe don't need this check?
            cout << "Binary file should have an integer number of 4-byte instructions";
            return 1;
        }
        for (int addr = 0; addr <= size; addr++){
            myMem->setMemValue(addr, memblock[addr], BYTE_SIZE);
        }
        // delete file from real memory. Now use MemoryStore object
        delete[] memblock;

        // TODO: maybe use myMem print function to check that mem is being set correctly
        // or is it good enough to just look at mem_state.out?
    }
    else {
        cout << "Unable to open file";
        dump(regs, myMem);
        return 1;
    }

    //-------------------------------------------------------------------------
    // Fetch/Decode/Execute Instructions
    //-------------------------------------------------------------------------
    while (true) {
        uint32_t instr;
        myMem->getMemValue(pc, instr, WORD_SIZE);

        printf("\n\nCurrent Instruction: %08x", instr);
        // NOTE: green card shows instr bitranges for little-endian... so either convert to little endian and use green card ranges (ex. convert to little endian then take opcode from 26-31), or keep in big endian and use inverted bitranges (ex. take opcode from 0-5)
        // I'll be keeping data in big endian and inverting green card bitranges
        uint32_t opcode = (instr & OPCODE_MASK) >> OPCODE_BITS;
        uint32_t rs     = (instr & RS_MASK) >> RS_BITS;
        uint32_t rt     = (instr & RT_MASK) >> RT_BITS;
        uint32_t rd     = (instr & RD_MASK) >> RD_BITS;
        uint32_t funct  = (instr & FUNCT_MASK) >> FUNCT_BITS;
        uint32_t imm    = (instr & IMM_MASK) >> IMM_BITS;
        uint32_t addr   = (instr & ADDR_MASK) >> ADDR_BITS;
        printf("\n\nopcode: %x\nrs:     %x\nrt:     %x\nrd:     %x\nfunct:  %x\nimm:    %x\naddr:   %x\n\n", opcode, rs, rt, rd, funct, imm, addr);
        
        if (instr == 0xfeedfeed) {
            dump(regs, myMem);
            return 0;
        }
        // switch (opcode) {
        //     case :
        // }

        // TODO: DELETE ONCE ALL CASES ARE IMPLEMENTED
        dump(regs, myMem);
        return 0;
    }
}


// A potential (not the only possible) outline of your program would be: 
// ----------------------------------------------------------------------------
// main (argc, argv) {
//     Create a memory store called myMem
//     Initialize registers to have value 0
//     Read bytes of binary file passed as parameter into appropriate memory locations 
//     Point the program counter to the first instruction
//     while (TRUE) {
//         Fetch current instruction from memory@PC 
//         Determine the instruction type
//         Get the operands
//         switch (instruction type) {
//             case 0xfeedfeed:
//                 RegisterInfo reg;
//                 Fill reg with the current contents of the registers 
//                 dumpRegisterState(reg); // this will create the mem_state.out file
//                 dumpMemoryState(myMem); // this will create the reg_state.out file
//                 return 0;
//             case INSTR1:
//                 Perform operation and update destination 
//                 register/memory/PC
//                 break;
//             ... 
//             default:
//                 fprintf(stderr,”Illegal operation...”);
//                 exit(127); 
//         }
//     }
// }
