#include "emulator.h"

#define MEM_SPACE 65536
#define DEBUG true;
#ifdef DEBUG
    #define debug_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define debug_info(M, ...)
#endif
#define ONS() printf("Operation not supported!"); exit(1)


bool halt = false;

//Registers
uint8_t A; //accumulator
uint8_t F; //flag

uint8_t B;
uint8_t C;

uint8_t D;
uint8_t E;

uint8_t H;
uint8_t L;

uint16_t PC; //program counter

uint16_t SP; //stack pointer

uint8_t mem[MEM_SPACE];
int main() {
    return emulate("out"); 
}
// adapted from https://stackoverflow.com/questions/18327439/printing-binary-representation-of-a-char-in-c
const char *byteToBinary(int x) {
    static char b[9];
    b[0] = '\0';
    int z;
    for (z = 128; z > 0; z >>= 1) {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }
    return b;
}
void dump(int length) {
    for (int i = 0; i < length; i++) {
        printf("%s\n", byteToBinary(mem[i]));
    } 
}
uint8_t *getRegister(uint8_t r) {
    debug_info("Getting register with index %i", r);
    switch (r) {
        case 0: return &B;
        case 1: return &C;
        case 2: return &D;
        case 3: return &E;
        case 4: return &H;
        case 5: return &L;
        case 6: ONS();
        case 7: return &A;
    }
    ONS();
    return NULL;
}

void setRegisterPair(uint8_t p, uint16_t newVal) {
    debug_info("Setting register pair %i to value %i", p, newVal);
    switch (p) {
        case 0:
            C = newVal & 0x00FF;
            B = (newVal & 0xFF00) >> 8;
            break;
        case 1:
            E = newVal & 0x00FF;
            D = (newVal & 0xFF00) >> 8;
            break;
        case 2:
            L = newVal & 0x00FF;
            H = (newVal & 0xFF00) >> 8;
            break;
        case 3:
            SP = newVal;
    }
}

uint16_t getRegisterPair(uint8_t p) {
    uint16_t res = 0;
    // TODO: is res needed?
    debug_info("Getting register pair %i", p); 
    switch (p) {
        case 0:
            return ((res | B) << 8) | C;
        case 1:
            return ((res | D) << 8) | E;
        case 2:
            return ((res | H) << 8) | L;
        case 3:
            return SP;
    }
    ONS();
}
#define BIT_Z  0b01000000 //zero bit
#define BIT_C  0b00000001 //carry bit
#define BIT_PO 0b00000100 //parity/overflow
#define BIT_S  0b10000000 //sign
#define BIT_SB 0b00000010 //subtract
#define BIT_H  0b00010000 //half carry
#define FIRST  0b00001111 //first 4 bits 
bool checkCondition(uint8_t c) {
    switch (c) {
        case 0: return !(BIT_Z & F); 
        case 1: return BIT_Z & F;
        case 2: return !(BIT_C & F);
        case 3: return (BIT_C & F);
        case 4: return !(BIT_PO & F);
        case 5: return (BIT_PO & F);
        case 6: return (BIT_S & F);
        case 7: return !(BIT_S & F); 
    }
    ONS();
}
void setFlags(uint8_t oldA, bool subtracted) {
    //preserve F5 and F3, since undocumented
    F = 0b00101000 & F;
    //set sign flag by copying MSB of A
    F = BIT_S & A;
    //Check if A is nonzero and set zero flag accordingly
    F = A ? F : (BIT_Z | F);
    //Set carry if A had a carry operation -- equivalent to A less than oldA
    //if addition, A > oldA if subtraction 
    //Half carry same way -- if first half of A < first half oldA
    //if addition, vice versa for subtraction
    if (subtracted) {
        F = A > oldA ? (F | BIT_C) : F;
        F = F | BIT_SB; //set subtract bit while we're at it
        F = ((A & FIRST) > (oldA & FIRST)) ? (F | BIT_H) : F;  
    } else {
        F = A < oldA ? (F | BIT_C) : F;
        F = ((A & FIRST) < (oldA & FIRST)) ? (F | BIT_H) : F;  
    }
    //TODO: figure out parity/overflow 
}

void outputToPort(uint8_t portAddr, uint8_t value) {
    //use port 0 as terminal output 
    switch (portAddr) {
        case 0: printf("%c", value); return;
        case 1: printf("[HARDWARE PORT 1]: %i\n", value); return;
    }
    ONS();
}

void nop() {
    //for debug right now
    printf("Nopped\n");
}
void inc(uint8_t y) {
    debug_info("Incrementing register %i", y);
    uint8_t *reg = getRegister(y);
    uint8_t old = *reg;
    (*reg)++;
    PC++;
    setFlags(old, false); 
}
void dec(uint8_t y) {
    debug_info("Decrementing register %i", y);
    uint8_t *reg = getRegister(y);
    uint8_t old = *reg;
    (*reg)--;
    PC++;
    setFlags(old, true);
}
//load an immediate operand
void ldImOp(uint8_t y) {
    debug_info("Loading immediate operand into register %i", y);
    uint8_t *reg = getRegister(y);
    PC++;
    (*reg) = mem[PC];
    PC++;
}
//load an immediate operand, 16 bits
void ldImOp16(uint8_t p) {
    debug_info("Loading immediate operand into 16-bit register %i", p);
    PC++;
    uint16_t lsb = mem[PC];
    PC++;
    uint16_t msb = mem[PC]; 
    uint16_t value = lsb | (msb << 8);
    debug_info("Loaded value %i", value);
    setRegisterPair(p, value);
    PC++;
}
// add two 16 bit registers
// TODO: 16 bit flag set
void add16(uint8_t p) {
    //adds register pair p to HL
    PC++;
    uint16_t hl = getRegisterPair(2);
    uint16_t summand = getRegisterPair(p);
    setRegisterPair(2, hl + summand);  
}
//OUT (n), A
void outImOp() {
    PC++;
    uint8_t portAddr = mem[PC];
    PC++;
    outputToPort(portAddr, A); 
}
void jp(uint8_t y) {
    if (checkCondition(y)) {
        PC++;
        uint16_t first = mem[PC];
        PC++;
        uint16_t second = mem[PC]; 
        second = first | second << 8;
        PC = second;
    } else {
        PC += 3;
    } 
}
void jpNc() {
    PC++;
    uint16_t first = mem[PC];
    PC++;
    uint16_t second = mem[PC];
    second = first | second << 8; 
    PC = second; 
}
void alu8(uint8_t operation, uint8_t value) {
    uint8_t oldA = A;
    switch (operation) {
        case 0: //ADD A
            A = A + value; setFlags(oldA, false); break;
        case 1: //ADC A
            A = value + (F & BIT_C); setFlags(oldA, false); break;
        case 2: //SUB
            A -= value; setFlags(oldA, true); break;
        case 3: //SBC A
            A -= value + (F & BIT_C); setFlags(oldA, true); break;
        case 4: //AND 
            A = A & value; setFlags(oldA, false); break;
        case 5: //XOR 
            A = A ^ value; setFlags(oldA, false); break;
        case 6: //OR
            A = A | value; setFlags(oldA, false); break;
        case 7: //CP
            A = A - value; setFlags(oldA, true); A = oldA; break; 
    }
    PC++;
}
void alu8ImOp(uint8_t y) {
    PC++;
    alu8(y, mem[PC]);  
}
void x0() {
    uint8_t y = (mem[PC] & 0b00111000) >> 3;
    uint8_t z = mem[PC] & 0b00000111;
    uint8_t q = mem[PC] & 0b00001000; //since only 1 bit, practically a boolean
    uint8_t p = (mem[PC] & 0b00110000) >> 4;
    switch (z) {
        case 0:
            switch (y) {
                case 0: nop(); return; 
                default: ONS();
            } 
        case 1:
            if (q == 0) {
                ldImOp16(p); 
            } else {
                add16(p);
            }; return;
        case 4: inc(y); return; 
        case 5: dec(y); return; 
        case 6: ldImOp(y); return; 
        default: ONS();
    }
}
void x1() {
    //All commands for x = 1 are LD [y][z]
    uint8_t y = (mem[PC] & 0b00111000) >> 3;
    uint8_t z = mem[PC] & 0b00000111;
    if (y == 6 && z == 6) {
        //halt
        halt = true;
        return; 
    }
    debug_info("Loading register %i into register %i", y, z);
    uint8_t *yReg = getRegister(y);
    uint8_t *zReg = getRegister(z);
    *yReg = *zReg;
    PC++;
}
void x2() {
    //All commands for x = 2 are ALU ops
    uint8_t y = (mem[PC] & 0b00111000) >> 3;
    uint8_t z = mem[PC] & 0b00000111;
    uint8_t *reg = getRegister(z);
    alu8(y, *reg);
}
void x3() {
    uint8_t y = (mem[PC] & 0b00111000) >> 3;
    uint8_t z = mem[PC] & 0b00000111;
    switch (z) {
        case 2:
            jp(y); return;
        case 3:
            switch (y) {
                case 0: jpNc(); return;
                case 2: outImOp(); return; 
            }
        case 6: alu8ImOp(y); return;
        default: ONS();
    }

}

void execute() {
    debug_info("Byte at PC: %s", byteToBinary(mem[PC])); 
    uint8_t x = (mem[PC] & 0b11000000) >> 6;
    switch (x) {
        case 0: x0(); break;
        case 1: x1(); break;
        case 2: x2(); break;
        case 3: x3(); break;
    }
}
int emulate(char *codeFile) {
    int codeLength = 0;
    FILE *code = fopen(codeFile, "rb");
    fseek(code, 0, SEEK_END);  
    codeLength = ftell(code); 
    rewind(code);
    if (codeLength > MEM_SPACE) {
        printf("Error! Program too large for memory.\n");
        exit(1);
    }
    fread(mem, codeLength, 1, code);
    dump(codeLength);
    PC = 0;
    int i = 0;
    while (PC < codeLength && !halt) { 
        execute();
        i++;
    }
    printf("Executed %i instructions before halting\n", i);
    return 0;
}
