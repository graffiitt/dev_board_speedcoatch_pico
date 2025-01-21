#ifndef UI_SPINBOX_INT_H
#define UI_SPINBOX_INT_H

#include "display/disp_task.h"

typedef struct
{
    int16_t x;
    int16_t y;
    char *nameItem;
    int data;
    int min;
    int max;
} spinbox_int;

void drawSpinBox(spinbox_int *box, bool selected);
void incrementSpinBox(spinbox_int *box);
void decrementSpinBox(spinbox_int *box);

#endif