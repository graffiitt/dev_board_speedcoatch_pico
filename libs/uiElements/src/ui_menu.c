#include "uiElements/ui_menu.h"
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
    uint8_t x = 25;
    uint8_t y = 37;

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

            getText_24()->x = WIDTH - text_str_width(getText_24(), cvector_at(*currentMenu->items, item)->descrItem) - 10;
            text_str(getText_24(), cvector_at(*currentMenu->items, item)->descrItem);
            getText_24()->y += text_height(getText_24()) + 7;
            getText_24()->x = x;
        }
    }
    getText_24()->y = y + (text_height(getText_24()) + 7) * (currentMenu->current_row > (NUM_DISPLAY_ROWS - 1) ? (NUM_DISPLAY_ROWS - 1) : currentMenu->current_row);

    getText_24()->x = 0;
    text_char(getText_24(), '>');
}

static void actionButtonUP(enum BUTTON_ACTION act)
{
    switch (act)
    {
    case HOLDING:
        menu_item_t *item = cvector_at(*currentMenu->items, currentMenu->current_row);
        if (item->action != NULL)
            item->action(MENU_ACTIONS_DOWN);
        break;
    case SHORT:
        if (currentMenu->current_row > cvector_size(*currentMenu->items) - 1)
        {
            currentMenu->current_row = cvector_size(*currentMenu->items) - 1;
        }
        else
        {
            currentMenu->current_row++;
        }
        drawDisplay();
        break;
    }
}

static void actionButtonDown(enum BUTTON_ACTION act)
{
    switch (act)
    {
    case HOLDING:
        menu_item_t *item = cvector_at(*currentMenu->items, currentMenu->current_row);
        if (item->action != NULL)
            item->action(MENU_ACTIONS_UP);
        break;
    case SHORT:
        if (currentMenu->current_row > 0)
            currentMenu->current_row--;
        drawDisplay();
        break;
    }
}

static void actionButtonSelect(enum BUTTON_ACTION act)
{
    switch (act)
    {
    case PUSH:
        break;
    case SHORT:
        menu_item_t *item = cvector_at(*currentMenu->items, currentMenu->current_row);
        if (item->action != NULL)
            item->action(MENU_ACTIONS_RUN);
        if (item->nextMenu != NULL)
            setMenu(item->nextMenu);
    }
}

static void actionBackBt()
{
    if (currentMenu->prevMenu == NULL)
        return;
    setMenu(currentMenu->prevMenu);
}

void setMenu(menu_desc_t *menu)
{
    currentMenu = menu;
    drawStatusStr(currentMenu->name_menu);
    drawDisplay();
}

void setupCallbacksMenu()
{
    actionBack = actionBackBt;
    setButtonHandler(1, actionButtonDown);
    setButtonHandler(2, actionButtonUP);
    setButtonHandler(3, actionButtonSelect);
}