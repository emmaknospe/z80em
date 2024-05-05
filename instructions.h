#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "label.h"


int convertInstruction(FILE *assembly, char *outBuffer, uint16_t *addr, Label *labels); 

#endif
