#ifndef SPINBOX_SCREEN_H
#define SPINBOX_SCREEN_H

#include "uiElements/ui_spinbox_int.h"
#include "display/disp_task.h"
#include "../cvector.h"

typedef struct
{
    char *nameItem;
    cvector(spinbox_int) *items;
    int current_item; // id item
    bool save;
} spinbox_screen_t;

// void drawSpinBoxScreen();
void setSpinBoxScreen(spinbox_screen_t *screen);

#endif