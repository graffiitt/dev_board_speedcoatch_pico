#ifndef UI_ELEMENTS
#define UI_ELEMENTS

#include "../cvector.h"
#include "button.h"

typedef void (*ui_item_handler)(void);

enum MENU_ACTIONS
{
    run,
    up,
    down
};

typedef struct
{
    char *nameItem;
    char *descrItem;
    /// @brief
    /// call from push current item
    ui_item_handler func[3];
} menu_item_t;

typedef struct menu_descrobe_t menu_desc_t;

struct menu_describe_t
{
    char *name_menu;
    uint8_t current_item_select;
    cvector(menu_item_t) * items;

    menu_desc_t *prevMenu;
};

void drawMenu(menu_desc_t *mn);
void appendItemToBack(menu_desc_t *mn, menu_item_t *item);

// handlers for buttons
void actionButtonUP();
#endif