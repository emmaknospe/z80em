#include "compiler.h"

bool in(char c, char *string);
int main() {
    compile("test.asm", "out");
}
/*
 * String equality checker for readability
 */
bool eq(char *stringA, char *stringB) {
    return !strcmp(stringA, stringB);
}
bool in(char c, char *string) {
    for (;*string != 0; string++) {
        if (c == *string) {
            return true;
        }
    }
    return false; 
}
char readBuffer[256]; 
int scanToken(FILE *file, int *lines) {
    char c;
    int bufLength = 0;
    while ((c = fgetc(file)) != EOF) {
        switch (c) {
            case '\n':
                (*lines)++;
            case ' ':
            case ',':
                if (bufLength != 0) {
                    readBuffer[bufLength] = 0;
                    return 0;                     
                } 
                break;
            case ';':
                while ((c = fgetc(file)) != '\n') {
                    if (c == EOF) {
                        break;
                    } 
                } 
                (*lines)++;
                break;
            default:
                readBuffer[bufLength] = c;
                bufLength++;
                break;
        }
    }
    if (bufLength != 0) {
        readBuffer[bufLength] = 0;
        return 0;
    } else { 
        return EOF;
    }
}
/*
 * Prints error message if token not found.
 */
int getExpected(FILE *file, int *lines, char *message) {
    int status = scanToken(file, lines);
    if (status == EOF) {
        printf("Error on line %i: %s\n", *lines, message);
        exit(1);
    }
    return status;
}
void compile(char *assemblyFilename, char *machineFilename) {
    FILE *assembly = fopen(assemblyFilename, "r");
    if (assembly == NULL) {
        printf("Error! Could not find file %s\n", assemblyFilename);
    }
    int status;
    enum CSTATE {STD, ORG, BYTE, WORD, EQU} state = STD;
    Label *labels = NULL;
    // First pass: get labels
    uint16_t startAddr = 0;
    uint16_t addr = 0;
    uint16_t temp = 0;
    int lines = 0;
    uint8_t outBuffer[4];
    while ((status = scanToken(assembly, &lines)) != EOF) {
        switch (state) {
            case STD:
                if (in(':', readBuffer)) {
                    //signifies label definition    
                    labels = makeAndAppendLabel(readBuffer, addr, labels); 
                } else if (in('.', readBuffer)) {
                    if (eq(readBuffer, ".ORG")) {
                        state = ORG; 
                    }
                    if (eq(readBuffer, ".BYTE")) {
                        state = BYTE;
                    }
                    if (eq(readBuffer, ".WORD")) {
                        state = WORD;
                    }
                } else {
                    addr += convertInstruction(assembly, outBuffer, readBuffer, NULL);
                }
                break;
            case ORG:
                startAddr = (uint16_t) atoi(readBuffer);
                addr = startAddr;
                state = STD;
                break;
            case BYTE: 
                state = STD;
                break;
            case WORD: 
                state = STD;
                break;
            case EQU: 
                printf("Compiler instruction .EQU not yet supported!\n");
                exit(1);
                state = STD;
                break;
        }
    }
    printf("First pass complete.\n"); 
    rewind(assembly);
    FILE *out = fopen(machineFilename, "wb");
    if (out == NULL) {
        printf("Error! Could not write to file %s\n", machineFilename);
    }
    while ((status = scanToken(assembly, &lines)) != EOF) {
        switch (state) {
            case STD:
                if (!in(':', readBuffer) && !in('.', readBuffer)) {
                    int size = convertInstruction(assembly, outBuffer, readBuffer, labels);
                    fwrite(outBuffer, size, sizeof(uint8_t), out); //standard line, compile
                } else if (in('.', readBuffer)) {
                    if (eq(readBuffer, ".ORG")) {
                        state = ORG; 
                    }
                    if (eq(readBuffer, ".BYTE")) {
                        state = BYTE;
                    }
                    if (eq(readBuffer, ".WORD")) {
                        state = WORD;
                    }
                }
                break; 
            case ORG:
                startAddr = (uint16_t) atoi(readBuffer);
                addr = startAddr;
                state = STD;
                break;
            case BYTE: 
                outBuffer[0] = (uint8_t) atoi(readBuffer);
                fwrite(outBuffer, 1, sizeof(uint8_t), out);
                state = STD;
                break;
            case WORD: 
                temp = (uint16_t) atoi(readBuffer);
                outBuffer[0] = (uint8_t) temp;
                outBuffer[1] = (uint8_t) (temp >> 8);
                fwrite(outBuffer, 2, sizeof(uint8_t), out); 
                state = STD;
                break;
            case EQU: break;
        }
    }
    printf("Successfully compiled %i lines!\n", lines);
}

