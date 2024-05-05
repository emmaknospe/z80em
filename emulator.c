#include "emulator.h"

#define MEM_SPACE 65536
#define DISASSEMBLE true
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)
#define ONS printf("Operation not supported!"); exit(1)

//Registers
char A; //accumulator
char F; //flag

char B;
char C;

char D;
char E;

char H;
char L;

uint16_t PC;

char mem[MEM_SPACE];
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
char *getRegister(char r) {
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
bool checkCondition(char c) {
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
void inc(char y) {
    char *reg = getRegister(y);
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
void dec(char y) {
    char *reg = getRegister(y);
    (*reg)--;
    PC++;
}
void ldIm(char y) {
    char *reg = getRegister(y);
    PC++;
    (*reg) = mem[PC];
    PC++;
}
void jp(char y) {
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
    char y = (mem[PC] & 0b00111000) >> 3;
    char z = mem[PC] & 0b00000111;
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
    char y = (mem[PC] & 0b00111000) >> 3;
    char z = mem[PC] & 0b00000111;
    ONS;
}
void x2() {
    char y = (mem[PC] & 0b00111000) >> 3;
    char z = mem[PC] & 0b00000111;
    ONS;
}
void x3() {
    char y = (mem[PC] & 0b00111000) >> 3;
    char z = mem[PC] & 0b00000111;
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
    char x = (mem[PC] & 0b11000000) >> 6;
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
