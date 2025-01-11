#include "uiElements/ui_menu.h"
#include <sharpdisp/bitmaptext.h>
#include "button.h"

extern void (*actionBack)(void);

static menu_desc_t *currentMenu;

/// @brief painter func
/// call from drawDisplay() in disp_task
void drawMenu()
{
    if (cvector_size(*currentMenu->items) == 0)
        return;

    // coordinates for start draw menu;
    uint8_t x = 7;
    uint8_t y = 16;

    getText_24()->x = x;
    getText_24()->y = y;
    if (currentMenu->current_row > cvector_size(*currentMenu->items) - 1)
    {
        currentMenu->current_row = cvector_size(*currentMenu->items) - 1;
    }

    for (uint8_t i = 0; i < NUM_DISPLAY_ROWS; i++)
    {
        int item = currentMenu->current_row > (NUM_DISPLAY_ROWS - 1) ? (currentMenu->current_row - NUM_DISPLAY_ROWS + 1) + i : i;
        if (item < cvector_size(*currentMenu->items))
        {
            text_str(getText_24(), cvector_at(*currentMenu->items, item)->nameItem);
            // st7567_WriteString(x, y, cvector_at(*items, item)->text, FontStyle_veranda_9);
            getText_24()->y += text_height(getText_24());
            getText_24()->x = x;
            // y += 12;
        }
    }
    getText_24()->y = 16 + text_height(getText_24()) * (currentMenu->current_row > (NUM_DISPLAY_ROWS - 1) ? (NUM_DISPLAY_ROWS - 1) : currentMenu->current_row);

    getText_24()->x = 0;
    text_char(getText_24(), '>');
    // st7567_WriteChar(0, y, '>', FontStyle_veranda_9);
}

void actionButtonUP(enum BUTTON_ACTION act)
{
    switch (act)
    {
    case PUSH:

        break;
    case SHORT:
        currentMenu->current_row++;
        drawDisplay();
        break;
    }
}

void actionButtonDown(enum BUTTON_ACTION act)
{
    switch (act)
    {
    case PUSH:

        break;
    case SHORT:
        if (currentMenu->current_row > 0)
            currentMenu->current_row--;
        drawDisplay();
        break;
    }
}

void actionButtonSelect(enum BUTTON_ACTION act)
{
}

void actionBackBt()
{
    if (currentMenu->prevMenu == NULL)
        return;
}

void setMenu(menu_desc_t *menu)
{
    currentMenu = menu;
    actionBack = actionBackBt;
    setButtonHandler(1, actionButtonDown);
    setButtonHandler(2, actionButtonUP);
    setButtonHandler(3, actionButtonSelect);
}