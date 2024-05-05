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
    bool completeExit = false;
    while ((c = fgetc(file)) != EOF) {
        switch (c) {
            case '\n':
                (*lines)++;
                if (bufLength != 0) {
                    ungetc(c, file); //necessary so our instruction parser doesn't overflow onto the next line
                    readBuffer[bufLength] = 0;
                    return 0;
                }
                break;
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
    char c;
    int lines = 0;
    uint8_t outBuffer[4];
    while ((status = scanToken(assembly, &lines)) != EOF) {
        if (in(':', readBuffer)) {
            //signifies label definition    
            labels = makeAndAppendLabel(readBuffer, addr, labels); 
        } else if (in('.', readBuffer)) {
            if (eq(readBuffer, ".ORG")) {
                status = scanToken(assembly, &lines);
                if (status == EOF) {
                    break;
                }
                startAddr = (uint16_t) atoi(readBuffer);
                addr = startAddr;
            }
            if (eq(readBuffer, ".BYTE")) {
                status = scanToken(assembly, &lines);
                if (status == EOF) {
                    break;
                }

            }
            if (eq(readBuffer, ".WORD")) {
                status = scanToken(assembly, &lines);
                if (status == EOF) {
                    break;
                }
            }
        } else {
            addr += convertInstruction(assembly, outBuffer, readBuffer, NULL, lines, true);
        }
        //finish reading the line:
        while ((c = fgetc(assembly)) != EOF && c != '\n') {};
        if (c == EOF) {
            printf("breaking\n");
            break;
        }
        lines++;
    }
    printf("First pass complete.\n"); 
    rewind(assembly);
    //printLabels(labels);
    lines = 0;
    FILE *out = fopen(machineFilename, "wb");
    if (out == NULL) {
        printf("Error! Could not write to file %s\n", machineFilename);
    }
    while ((status = scanToken(assembly, &lines)) != EOF) {
        if (in(':', readBuffer)) {
            //signifies label definition    
            labels = makeAndAppendLabel(readBuffer, addr, labels); 
        } else if (in('.', readBuffer)) {
            if (eq(readBuffer, ".ORG")) {
                status = scanToken(assembly, &lines);
                if (status == EOF) {
                    break;
                }
                startAddr = (uint16_t) atoi(readBuffer);
                addr = startAddr;
            }
            if (eq(readBuffer, ".BYTE")) {
                status = scanToken(assembly, &lines);
                if (status == EOF) {
                    break;
                }
                outBuffer[0] = (uint8_t) atoi(readBuffer);
                fwrite(outBuffer, 1, sizeof(uint8_t), out);
            }
            if (eq(readBuffer, ".WORD")) {
                status = scanToken(assembly, &lines);
                if (status == EOF) {
                    break;
                }
                temp = (uint16_t) atoi(readBuffer);
                outBuffer[0] = (uint8_t) temp;
                outBuffer[1] = (uint8_t) (temp >> 8);
                fwrite(outBuffer, 2, sizeof(uint8_t), out); 
            }
        } else {
            int size = convertInstruction(assembly, outBuffer, readBuffer, labels, lines, false);
            lines++;
            fwrite(outBuffer, size, sizeof(uint8_t), out); //standard line, compile
        }
        //finish reading the line:
        while ((c = fgetc(assembly)) != EOF && c != '\n') {};
        if (c == EOF) {
            break;
        }
        lines++;
    }
    printf("Successfully compiled %i lines!\n", lines);
}

