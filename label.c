#include "label.h"



Label *makeAndAppendLabel(char *label, uint16_t value, Label *labels) {
    Label *ret = malloc(sizeof(Label));
    //remove 1 character: semicolon
    char *labelString = malloc(sizeof(char) * (strlen(label) - 1));
    int i;
    for (i = 0; *label != ':'; i++, label++) {
        labelString[i] = *label;
    } 
    labelString[i] = 0; //null terminate
    ret->label = labelString; 
    ret->value = value;
    ret->next = labels;
    return ret;
}
void printLabels(Label *labels) {
    for (; labels != NULL; labels = labels->next) {
        printf("%s: %i\n", labels->label, labels->value);
    }    
}


