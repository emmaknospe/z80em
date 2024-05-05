#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "label.h"


int convertInstruction(FILE *assembly, uint8_t *outBuffer, char *instruction, Label *labels, int line, bool firstPass); 

#endif
