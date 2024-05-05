#include "emulator.h"

#define MEM_SPACE 65536
#define DISASSEMBLE true
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)
#define ONS printf("Operation not supported!"); exit(1)

//Registers
uint8_t A; //accumulator
uint8_t F; //flag

uint8_t B;
uint8_t C;

uint8_t D;
uint8_t E;

uint8_t H;
uint8_t L;

uint16_t PC;

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
    switch (r) {
        case 0: return &B;
        case 1: return &C;
        case 2: return &D;
        case 3: return &E;
        case 4: return &H;
        case 5: return &L;
        case 6: ONS;
        case 7: return &A;
    }
    ONS;
    return NULL;
}
#define BIT_Z  0b01000000
#define BIT_C  0b00000001
#define BIT_PO 0b00000100
#define BIT_S  0b10000000
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
    ONS;
}
void nop() {
    printf("NOP\n");
    exit(1);
}
void inc(uint8_t y) {
    uint8_t *reg = getRegister(y);
    uint8_t old = *reg;
    (*reg)++;
    PC++;
    //probably split flag logic into its own function
    F = 0b00101000 & F;
    F = BIT_S & A;
    F = A ? F : (0b01000000 | F);
    if (old & 0b00001000 && !((*reg) & 0b00001000)) {
        F = F | 0b00010000;
    } 
    // parity overflow omitted 
    if (old == 255) {
        F = F | 0b00000001; 
    } 
}
//TODO implement changing flags
void dec(uint8_t y) {
    uint8_t *reg = getRegister(y);
    (*reg)--;
    PC++;
}
void ldIm(uint8_t y) {
    uint8_t *reg = getRegister(y);
    PC++;
    (*reg) = mem[PC];
    PC++;
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
void x0() {
    uint8_t y = (mem[PC] & 0b00111000) >> 3;
    uint8_t z = mem[PC] & 0b00000111;
    switch (z) {
        case 0:
            switch (y) {
                case 0: nop(); return; 
                default: ONS;
            } 
        case 4: inc(y); return; 
        case 5: dec(y); return; 
        case 6: ldIm(y); return; 
        default: ONS;
    }
}
void x1() {
    uint8_t y = (mem[PC] & 0b00111000) >> 3;
    uint8_t z = mem[PC] & 0b00000111;
    ONS;
}
void x2() {
    //All commands for x = 2 are ALU ops
    uint8_t y = (mem[PC] & 0b00111000) >> 3;
    uint8_t z = mem[PC] & 0b00000111;
    uint8_t *reg = getRegister(z);
    uint16_t tmp; //recall: cannot declare variables inside switch statements
    switch (y) {
        case 0: //ADD A
            tmp = A + *reg; 
            F = (tmp > 255) ? (F | BIT_C) : F; //set carry flag if overflow
            A = tmp;
            break;
        case 1: //ADC A
            tmp = *reg + (F & BIT_C); break;
            F = (tmp > 255) ? (F | BIT_C) : F; //set carry flag if overflow
            A = tmp;
            break;
        case 2: //SUB
            A -= *reg; break;
        case 3: //SBC A
            A -= *reg + (F & BIT_C); break;
        case 4: //AND 
            A = A & *reg; break;
        case 5: //XOR 
            A = A ^ *reg; break;
        case 6: //OR
            A = A | *reg; break;
        case 7: //CP
            A = A - *reg; //flag set here
            break;
    }
    ONS;
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
            }
        default: ONS;
    }

}

void execute() {
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
    while (PC < codeLength) { 
        execute();
    }
    return 0;
}
