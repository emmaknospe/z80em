#include "instructions.h"

int LINE;
bool FIRST_PASS;

#define error(msg) printf("Error on line %i: %s\n", LINE, msg); exit(1) 

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
char buffer[64];
char arg1[64];
char arg2[64];

void replaceLabelsAndCopy(Label *labels, char *outBuffer) {
    int i = 0;
    int len;
    for (char *subString = buffer; *subString != 0; subString++) {
        for (; labels != NULL; labels = labels->next) {
            if (eq(subString, labels->label)) {
                len = strlen(labels->label);
                if (i >= 64) {
                    error("Replace buffer overflow!");
                }
                subString += len; 
                int ret = sprintf(outBuffer + i, "%i", labels->value);
                if (ret < 0) {
                    error("sprintf errored on write!");
                }
                i += ret;
                goto loopEnd;
            } 
        }
        outBuffer[i] = *subString; 
        i++;
        loopEnd: ; 
    }
    outBuffer[i] = 0;
}

void loadBuf(FILE *a, Label *labels) {
    int i = 0;
    char c;
    bool onArg1 = true;
    while ((c = fgetc(a)) != EOF && c != ';' && c != '\n') {
        if (c == ',') {
            onArg1 = false; 
            buffer[i] = 0;
            replaceLabelsAndCopy(labels, arg1); 
            i = 0;
        } else if (c != ' ') {
            buffer[i] = c;
            i++;
        }
    }
    if (c == '\n') {
        //put it back on, we need it in the main parse area
        ungetc(c, a);
    }
    buffer[i] = 0;
    if (!onArg1) {
        replaceLabelsAndCopy(labels, arg2);
    } else {
        replaceLabelsAndCopy(labels, arg1);
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
    error("Unrecognized condition!");
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
    error("Unrecognized register!");
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
//is data of the form <label>?
bool isLabel(char *data) {
    if (!isLetter(*data) || isRegister(data) || isCond(data)) {
        return false;
    }
    for (; *data != 0; data++) {
        if (!(isLetter(*data) || isDigit(*data))) {
            return false;
        } 
    }
    return true;
}
// is data of the form <digits>?
bool isNumber(char *data) {
    for (; *data != 0; data++) {
        if (!isDigit(*data)) {
            return false;
        }
    }
    return true;
}
bool isData(char *data) {
    return isNumber(data) || (FIRST_PASS && isLabel(data));
}
// is addrData of the form (<label>)?
bool isAddressedLabel(char *addrData) {
    if (addrData[0] != '(') {
        return false;
    }
    if (!isLetter(addrData[1])) {
        return false;
    }
    addrData++;
    for (; *addrData != 0 && *addrData != ')'; addrData++) {
        if (!(isDigit(*addrData) || isLetter(*addrData))) {
            return false;
        }
    }
    if (*addrData != ')') {
        return false;
    }
    addrData++;
    if (*addrData != 0) {
        return false;
    }
    return true;

}
// is addrData of the form (<digits>)? asking for a friend
bool isAddressedNumber(char *addrData) {
    if (addrData[0] != '(') {
        return false;
    }
    addrData++;
    for (; *addrData != 0 && *addrData != ')'; addrData++) {
        if (!isDigit(*addrData)) {
            return false;
        }
    }
    if (*addrData != ')') {
        return false;
    }
    addrData++;
    if (*addrData != 0) {
        return false;
    }
    return true;
}

bool isAddressedData(char *data) {
    return isAddressedNumber(data) || (FIRST_PASS && isAddressedLabel(data));
}


int nop(FILE *a, uint8_t *o) {
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
int ld(FILE *a, uint8_t *o) {
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
            error("Load expects two arguments!");
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
    error("Unsupported operation for LD!");
    return 0;
} 

int inc(FILE *a, uint8_t *o) {
    if (isRegister(arg1)) {
        int regCode = getRegCode(arg1) << 3;
        o[0] = 0b00000100 | regCode; 
        return 1;
    }
    error("Unsupported operation for INC!");
    return 0;
}
int dec(FILE *a, uint8_t *o) {
    if (isRegister(arg1)) {
        int regCode = getRegCode(arg1) << 3;
        o[0] = 0b00000101 | regCode; 
        return 1;
    }
    error("Unsupported operation for DEC!");
    return 0;
}

int jp(FILE *a, uint8_t *o) {
    if (isCond(arg1)) {
        if (isData(arg2)) {
            int condCode = getCondCode(arg1) << 3;
            o[0] = 0b11000010 | condCode;
            uint16_t label = atoi(arg2);
            o[1] = label & 0xff;
            o[2] = label >> 8;
            return 3;
        } else {
            error("Unsupported second argument for jp!");    
        }
    } else if (isData(arg1) && eq(arg2, "")) {
        o[0] = 0xC3;
        int label = atoi(arg2);
        o[1] = label & 0xff;
        o[2] = label >> 8;
        return 3;
    }
    error("Unsupported operation for JP!");
    return 0;

}
int add(FILE *a, uint8_t *o) {
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
    error("Unsupported operation for ADD!");
    return 0;
}
int out(FILE *a, uint8_t *o) {
    if (isAddressedData(arg1)) {
        if (eq(arg2, "A")) {
            //out (port), A
            o[0] = 0xD3;
            char *a;
            //find close paren of addressed data 
            for (a = arg1; *a != ')'; a++) {}
            *a = 0; //replace close paren with null to terminate string
            char *b = arg1 + 1; //set up new pointer to directly past first open paren 
            //we now are set up to use atoi to parse the integer out
            o[1] = atoi(b);
            return 2;
        } else {
            error("Cannot perform immediate OUT op on registers other than A!");
        }
    } else if (eq(arg1, "(C)")) {
        if (isRegister(arg2)) {
            //out (c), reg
            o[0] = 0xED;
            //format: 01sss001
            o[1] = (0b01000001 | (getRegCode(arg2) << 3));
            return 2;
        }
    }
    error("Unsupported operation for OUT!");
}
int cp(FILE *a, uint8_t *o) {
    if (eq(arg1, "(HL)")) {
        if (eq(arg2, "")) {
            //cp (HL)
            o[0] = 0xBE;
            return 1;
        }
    } else if (isData(arg1)) {
        if (eq(arg2, "")) {
            //cp data
            o[0] = 0xFE;
            o[1] = atoi(arg1); 
            return 2;
        }
    }
    error("Unsupported operation for CP!");
}
int halt(FILE *a, uint8_t *o) {
    o[0] = 0x76;
    return 1;
}
char opStrings[69][10] = {
    "NOP",
    "LD",
    "INC",
    "DEC",
    "JP",
    "ADD",
    "OUT",
    "CP",
    "HALT",
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
    "ADC",
    "SUB",
    "SBC",
    "AND",
    "XOR",
    "OR",
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
int IMPLEMENTED = 9;
int (*operations[69])(FILE *, uint8_t *) = {
    nop,
    ld,
    inc,
    dec,
    jp,
    add,
    out,
    cp,
    halt
};


int convertInstruction(FILE *assembly, uint8_t *outBuffer, char *instruction, Label *labels, int line, bool firstPass) {
    LINE = line;
    FIRST_PASS = firstPass;
    loadBuf(assembly, labels);
    for (int i = 0; i < IMPLEMENTED; i++) {
        if (eq(instruction, opStrings[i])) {
            return operations[i](assembly, outBuffer);
        } 
    } 
    printf("Operation %s not supported!\n", instruction);
    exit(1);
} 
