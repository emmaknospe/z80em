#ifndef COMPILER_H
#define COMPILER_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "label.h"
#include "instructions.h"
/*
 * z80 spec from z80 Assembly Language Programming By Lance A. Leventhal
 */


void compile(char *assemblyFilename, char *machineFilename);

 

#endif
