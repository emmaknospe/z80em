#include "instructions.h"



char instrStrings[69][10] = {
    "NOP",
    "LD",
    "INC",
    "DEC",
    "RCLA",
    "EX",
    "ADD",
    "RRCA",
    "DJNZ",
    "RLA",
    "JR",
    "RRA",
    "CPL",
    "SCF",
    "CCF",
    "HALT",
    "ADC",
    "SUB",
    "SBC",
    "AND",
    "XOR",
    "OR",
    "CP",
    "RET",
    "POP",
    "JP",
    "CALL",
    "PUSH",
    "ADD",
    "RST",
    "RLC",
    "RRC",
    "RL",
    "RR",
    "SLA",
    "SRA",
    "SRL",
    "BIT",
    "RES",
    "SET",
    "POP",
    "OUT",
    "EXX",
    "IN",
    "NEG",
    "RETN",
    "IM",
    "RETI",
    "RRD",
    "RLD",
    "LDI",
    "CPI",
    "INI",
    "OUTI",
    "LDD",
    "CPD",
    "IND",
    "OUTD",
    "LDIR",
    "CPIR",
    "INIR",
    "OTIR",
    "LDDR",
    "CPDR",
    "INDR",
    "OTDR",
    "DI",
    "EI"
    "DAA"
};

char buf[128];
bool begins(char *a) {
    char *b = buf;
    for (; *a != 0; a++, b++) {
        if (*a != *b) {
            return false;
        }
    }
    return true;
}
bool letter(char a) {
    char *letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (; *letters != 0; letters++) {
        if (a == *letters) {
            return true;
        }
    }
    return false;
}
bool digit(char a) {
    char *digits = "1234567890";
    for (; *digits != 0; digits++) {
        if (a == *digits) {
            return true;
        }
    }
    return false;
}

uint16_t get16Data(int start) {
    char *c = buf[start];
    char databuf[32];
    if (digit(*c)) {
        int i = 0;
        for (; *c != ' ' && *c != 0; c++, i++) {
            databuf[i] = *c; 
            if (!digit(*c)) {
                printf("Data parsing error!\n");
                exit(1);
            }
        }
        databuf[i] = 0;
        return (uint16_t) atoi(databuf);
    }
}
void loadBuf(FILE *a) {
    int i = 0;
    char c;
    while ((c = fgetc(a)) != EOF && c != ';' && c != '\n') {
        if (c != ' ') {
            buf[i] = c;
            i++;
        }
    }
    buf[i] = 0;
}
int nop(FILE *a, char *o, uint16_t *addr, Label *labels) {
    o[0] = 0x00;
    *addr++;
    return 1; 
}
int ld(FILE *a, char *o, uint16_t *addr, Label *labels) {
    loadBuf(a);
    if (begins("BC,")) {
        o[0] = 0x01;
        uint16_t data = get16Data(3);
        o[1] = data & 0xFF;
        o[2] = value >> 8;
        return 3;
    } else if (begins("(BC),A")) {
        o[0];  
    }
} 

int (*process_instr[69])(FILE *, char *, uint16_t *, Label *) = {
    nop,
};


int convertInstruction(FILE *assembly, char *outBuffer, uint16_t *addr, Label *labels) {
    return 0;
} 
