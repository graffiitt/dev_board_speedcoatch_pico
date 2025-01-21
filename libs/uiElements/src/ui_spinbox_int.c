#include "uiElements/ui_spinbox_int.h"
#include "sharpdisp/bitmapshapes.h"
#include <stdio.h>

void drawSpinBox(spinbox_int *box, bool selected)
{
    getText_24()->x = box->x;
    getText_24()->y = box->y;
    text_str(getText_24(), box->nameItem);
    text_char(getText_24(), ':');

    char str[5];
    sprintf(str, "%d", box->data);
    if(selected)
    {
        bitmap_filled_rect(getBitmap(),  getText_24()->x,  getText_24()->y, text_str_width(getText_24(), str), 23);        
        getBitmap()->mode = BITMAP_WHITE;
        text_str(getText_24(), str);
        getBitmap()->mode = BITMAP_BLACK;
    }
    else 
    {
        text_str(getText_24(), str);
    }
}

void incrementSpinBox(spinbox_int *box)
{
    if (box->data < box->max)
        box->data++;
}

void decrementSpinBox(spinbox_int *box)
{
    if (box->min < box->data)
        box->data--;
}
