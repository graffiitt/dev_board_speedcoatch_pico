#ifndef UI_ELEMENTS
#define UI_ELEMENTS

#include "display/disp_task.h"
#include "../cvector.h"

#define NUM_DISPLAY_ROWS 5

enum MENU_ACTIONS
{
    MENU_ACTIONS_RUN,
    MENU_ACTIONS_UP,
    MENU_ACTIONS_DOWN,
    MENU_ACTIONS_SELECT_HOLD
};

typedef void (*ui_item_handler)(enum MENU_ACTIONS);
typedef struct menu_describe_t menu_desc_t;

typedef struct
{
    const char *nameItem;
    char *descrItem;
    /// @brief

   ui_item_handler action;
   // void (*setupScreen)(void);
    menu_desc_t *nextMenu;

    // TODO add callbacks for long button event
} menu_item_t;

struct menu_describe_t
{
    char *name_menu;
    uint8_t current_row;
    cvector(menu_item_t) * items;

    menu_desc_t *prevMenu;
};

void drawMenu();
void setMenu(menu_desc_t *menu);
void setupCallbacksMenu();

#endif