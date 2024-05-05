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
bool isData(char *string) {
    return strlen(string) > 2 && string[0] != '(';
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
    int status;
    enum CSTATE {STD, ORG, BYTE, WORD, EQU} state = STD;
    Label *labels = NULL;
    // First pass: get labels
    uint16_t startAddr = 0;
    uint16_t addr = 0;
    int lines = 0;
    char outBuffer[16];
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
                } else {
                    convertInstruction(assembly, outBuffer, &addr, NULL);
                }
            case ORG:
                startAddr = (uint16_t) atoi(readBuffer);
                addr = startAddr;
                state = STD;
                break;
            case BYTE: break;
            case WORD: break;
            case EQU: break;
        }
    }
    printLabels(labels); 
}

