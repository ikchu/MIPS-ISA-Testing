#include <iostream>
#include <fstream>
#include <iomanip>
#include "MemoryStore.h"
#include "RegisterInfo.h"

#define NUM_REGS 32

// NOTE: while MIPS technically has $t0-$t7 contiguous, then $s0-$s7 contiguous, then $t8-t9
// I will simply group all $t0-$t9 registers together contiguously in my regs array
enum RegOffsets {
    Z_OFFSET = 0,
    AT_OFFSET = 1,
    V_OFFSET = 2,
    A_OFFSET = 4,
    T_OFFSET = 8,
    S_OFFSET = 18,
    K_OFFSET = 26,
    GP_OFFSET = 28,
    SP_OFFSET = 29, 
    FP_OFFSET = 30,
    RA_OFFSET = 31
};

using namespace std;

int main(int argc, char **argv) {
    //-------------------------------------------------------------------------
    // Create MemoryStore object, Create registers and initialize to 0
    //-------------------------------------------------------------------------
    MemoryStore *myMem = createMemoryStore();
    uint32_t regs[NUM_REGS] = {0};

    //-------------------------------------------------------------------------
    // Parse arguments to obtain name of binary file
    //-------------------------------------------------------------------------
    if (argc != 2) {
        cout << "Invalid number of arguments";
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
        // transfer file line-by-line from real memory to abstract MemoryStore memory
        if (size % WORD_SIZE != 0) { // maybe don't need this check?
            cout << "Binary file should have an integer number of 4-byte instructions";
            return 1;
        }
        for (int addr = 0; addr + WORD_SIZE <= size; addr += WORD_SIZE){
            myMem->setMemValue(addr, memblock[addr], WORD_SIZE);
        }
        // delete file from real memory. Now use MemoryStore object
        delete[] memblock;
    }
    else {
        cout << "Unable to open file";
        return 1;
    }

    //-------------------------------------------------------------------------
    // Point PC to first instruction (address 0 of MemoryStore object)
    //-------------------------------------------------------------------------


    //-------------------------------------------------------------------------
    // Fetch/Decode/Execute Instructions
    //-------------------------------------------------------------------------


    // 0xfeedfeed code. Using it to test that I'm saving the bin file correctly
    RegisterInfo reg;
    int i;
    for (i = 0; i < NUM_REGS; i++) {
        switch (i) {
            case Z_OFFSET:                      // regs[0] - The $zero register (not included in RegisterInfo object)
                ;
            case AT_OFFSET:                     // regs[1] - The $at register.
                reg.at = regs[i];
            case V_OFFSET ... A_OFFSET-1:       // regs[2:3] - The $v registers.
                reg.v[i - V_OFFSET] = regs[i];
            case A_OFFSET ... T_OFFSET-1:       // regs[4:7] - The $a registers.
                reg.a[i - A_OFFSET] = regs[i];
            case T_OFFSET ... S_OFFSET-1:       // regs[8:17] - The $t registers.
                reg.t[i - T_OFFSET] = regs[i];
            case S_OFFSET ... K_OFFSET-1:       // regs[18:25] - The $s registers.
                reg.s[i - S_OFFSET] = regs[i];
            case K_OFFSET ... GP_OFFSET-1:      // regs[26:27] - The $k registers.
                reg.k[i - K_OFFSET] = regs[i];
            case GP_OFFSET:                     // regs[28] - The $gp register.
                reg.gp = regs[i];
            case SP_OFFSET:                     // regs[29] - The $sp register.
                reg.sp = regs[i];
            case FP_OFFSET:                     // regs[30] - The $fp register.
                reg.fp = regs[i];
            case RA_OFFSET:                     // regs[31] - The $ra register.
                reg.ra = regs[i];
        }
    }
    dumpRegisterState(reg);
    dumpMemoryState(myMem);
    return 0;
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