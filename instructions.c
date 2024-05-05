#include "instructions.h"



#define eq(stringA, stringB) !strcmp(stringA, stringB)
bool isLetter(char a) {
    char *letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (; *letters != 0; letters++) {
        if (a == *letters) {
            return true;
        }
    }
    return false;
}
bool isDigit(char a) {
    char *digits = "1234567890";
    for (; *digits != 0; digits++) {
        if (a == *digits) {
            return true;
        }
    }
    return false;
}

char arg1[64];
char arg2[64];
void loadBuf(FILE *a) {
    int i = 0;
    char c;
    bool onArg1 = true;
    while ((c = fgetc(a)) != EOF && c != ';' && c != '\n') {
        if (c == ',') {
            onArg1 = false; 
            arg1[i] = 0;
            i = 0;
        } else if (c != ' ') {
            if (onArg1) {
                arg1[i] = c;
            } else {
                arg2[i] = c;
            }
            i++;
        }
    }
    if (!onArg1) {
        arg2[i] = 0;
    } else {
        arg2[0] = 0;
    }
}

int getCondCode(char *cond) {
    if (eq(cond, "NZ")) {
        return 0;
    } else if (eq(cond, "Z")) {
        return 1;
    } else if (eq(cond, "NC")) {
        return 2;
    } else if (eq(cond, "C")) {
        return 3;
    } else if (eq(cond, "PO")) {
        return 4;
    } else if (eq(cond, "PE")) {
        return 5;
    } else if (eq(cond, "P")) {
        return 6;
    } else if (eq(cond, "M")) {
        return 7;
    }
    printf("Unrecognized condition!");
    exit(1);
}

bool isCond(char *cond) {
    if (eq(cond, "NZ")) {
        return true;
    } else if (eq(cond, "Z")) {
        return true;
    } else if (eq(cond, "NC")) {
        return true;
    } else if (eq(cond, "C")) {
        return true;
    } else if (eq(cond, "PO")) {
        return true;
    } else if (eq(cond, "PE")) {
        return true;
    } else if (eq(cond, "P")) {
        return true;
    } else if (eq(cond, "M")) {
        return true;
    }
    return false;
}
int getRegCode(char *reg) {
    if (eq(reg, "B")) {
        return 0;
    } else if (eq(reg, "C")) {
        return 1;
    } else if (eq(reg, "D")) {
        return 2;
    } else if (eq(reg, "E")) {
        return 3;
    } else if (eq(reg, "H")) {
        return 4;
    } else if (eq(reg, "L")) {
        return 5;
    } else if (eq(reg, "(HL)")) {
        return 6;
    } else if (eq(reg, "A")) {
        return 7;
    }
    printf("Unrecognized register!");
    exit(1);
}

bool isRegister(char *reg) {
    if (eq(reg, "B")) {
        return true;
    } else if (eq(reg, "C")) {
        return true;
    } else if (eq(reg, "D")) {
        return true;
    } else if (eq(reg, "E")) {
        return true;
    } else if (eq(reg, "H")) {
        return true;
    } else if (eq(reg, "L")) {
        return true;
    } else if (eq(reg, "A")) {
        return true;
    }
    return false;
}

bool isData(char *data) {
    for (; *data != 0; data++) {
        if (!isDigit(*data)) {
            return false;
        }
    }
    return true;
}



int nop(FILE *a, char *o) {
    o[0] = 0x00;
    return 1; 
}
/* 
 * Implemented:
 * ld reg,(HL)
 * ld A,(BC)
 * ld A,(DE)
 * ld (BC),A
 * ld (DE),A
 * ld (HL),reg
 * ld reg, reg
 * ld reg,data
 */
int ld(FILE *a, char *o) {
    loadBuf(a);
    if (eq(arg2, "(HL)")) {
        int regCode = getRegCode(arg1) << 3;
        o[0] = 0b01000110 | regCode; 
        return 1; 
    } else if (eq(arg1, "(HL)")) {
        int regCode = getRegCode(arg2) << 3;
        o[0] = 0b01110000 | regCode;
        return 1;
    } else if (eq(arg1, "A") && !isData(arg2)) {
        if (eq(arg2, "")) {
            printf("Error! Load expects 2 arguments."); 
            exit(1);
        } else if (eq(arg2, "(BC)")) {
            o[0] = 0x0A; 
            return 1;
        } else if (eq(arg2, "(DE)")) {
            o[0] = 0x1A;    
            return 1;
        } 
    } else if (eq(arg2, "A") && !isData(arg2)) {
        if (eq(arg1, "(BC)")) {
            o[0] = 0x02;
            return 1;
        } else if (eq(arg1, "(DE)")) {
            o[0] = 0x12;
            return 1;
        } 
    } if (isRegister(arg1)) {
        if (isRegister(arg2)) {
            int regCodeA = getRegCode(arg1) << 3;
            int regCodeB = getRegCode(arg2);
            o[0] = 0b01000000 | regCodeA | regCodeB;
            return 1;
        } else if (isData(arg2)) {
            int regCode = getRegCode(arg1) << 3;
            o[0] = 0b00000110 | regCode; 
            o[1] = (char) atoi(arg2);
            return 2;
        }
    } 
    printf("Error! Unsupported operation!");
    exit(1);
    return 0;
} 

int inc(FILE *a, char *o) {
    loadBuf(a);
    if (isRegister(arg1)) {
        int regCode = getRegCode(arg1) << 3;
        o[0] = 0b00000100 | regCode; 
        return 1;
    }
    printf("Error! Unsupported operation!");
    exit(1);
    return 0;
}
int dec(FILE *a, char *o) {
    loadBuf(a);
    if (isRegister(arg1)) {
        int regCode = getRegCode(arg1) << 3;
        o[0] = 0b00000101 | regCode; 
        return 1;
    }
    printf("Error! Unsupported operation!");
    exit(1);
    return 0;
}

int jp(FILE *a, char *o) {
    loadBuf(a);
    if (isCond(arg1)) {
        int condCode = getCondCode(arg1) << 3;
        o[0] = 0b11000010 | condCode;
        int label = atoi(arg2);
        o[1] = label & 0xff;
        o[2] = label >> 8;
        return 3;
    } if (isData(arg1) && eq(arg2, "")) {
        o[0] = 0xC3;
        int label = atoi(arg2);
        o[1] = label & 0xff;
        o[2] = label >> 8;
        return 3;
    }
    printf("Error! Unsupported operation!");
    exit(1);
    return 0;

}
int add(FILE *a, char *o) {
    loadBuf(a);
    if (eq(arg1, "A")) {
        if (isRegister(arg2)) {
            o[0] = 0b10000000 | getRegCode(arg2);
            return 1;
        } else if (isData(arg2)) {
            o[0] = 0xC6;
            o[2] = atoi(arg2); 
            return 2;
        }
    }
    printf("Error! Unsupported operation!");
    exit(1);
    return 0;
}
char opStrings[69][10] = {
    "NOP",
    "LD",
    "INC",
    "DEC",
    "JP",
    "ADD",
    "RCLA",
    "EX",
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
int IMPLEMENTED = 6;
int (*operations[69])(FILE *, char *) = {
    nop,
    ld,
    inc,
    dec,
    jp,
    add
};


int convertInstruction(FILE *assembly, char *outBuffer, char *instruction) {
    for (int i = 0; i < IMPLEMENTED; i++) {
        if (eq(instruction, opStrings[i])) {
            return operations[i](assembly, outBuffer);
        } 
    } 
    printf("Operation %s not supported!\n", instruction);
    exit(1);
} 
