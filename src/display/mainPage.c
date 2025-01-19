#include <sharpdisp/sharpdisp.h>
#include <uiElements/ui_menu.h>
#include "button.h"

static char *nameItems[] = {"display", "train", "memory", "settings", "watch", "item2", "item3"};
char *desc[] = {"qwerty"};
static cvector(menu_item_t) main_menu_items = NULL;
menu_desc_t main_menu = {"menu", 0, &main_menu_items, NULL};

extern menu_desc_t settings_menu;

void setupMainPage()
{
    menu_item_t item_1 = {nameItems[0], desc[0], NULL, NULL};
    cvector_push_back(main_menu_items, item_1);
    menu_item_t item_2 = {nameItems[1], NULL, NULL, NULL};
    cvector_push_back(main_menu_items, item_2);
    menu_item_t item_3 = {nameItems[2], NULL, NULL, NULL};
    cvector_push_back(main_menu_items, item_3);
    menu_item_t item_4 = {nameItems[3], NULL, NULL, &settings_menu};
    cvector_push_back(main_menu_items, item_4);
    menu_item_t item_5 = {nameItems[4], NULL, NULL, NULL};
    cvector_push_back(main_menu_items, item_5);
    menu_item_t item_6 = {nameItems[5], NULL, NULL, NULL};
    cvector_push_back(main_menu_items, item_6);
    menu_item_t item_7 = {nameItems[6], NULL, NULL, NULL};
    cvector_push_back(main_menu_items, item_7);
    
    // setup for first menu
    drawFunction = &drawMenu;
    setupCallbacks();
    setMenu(&main_menu);
}