#ifndef LABEL_H
#define LABEL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Label {
    char *label;
    uint16_t value; 
    struct Label *next;
};

typedef struct Label Label;


Label *makeAndAppendLabel(char *label, uint16_t value, Label *labels); 
void printLabels(Label *labels); 


#endif


