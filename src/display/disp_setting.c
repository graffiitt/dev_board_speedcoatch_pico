#include <sharpdisp/sharpdisp.h>
#include <uiElements/ui_menu.h>
#include "button.h"

static char *nameItems[] = {"bluetooth", "view elements", "gps info", "item1", "item2", "item3"};

static cvector(menu_item_t) main_settings_items = NULL;
extern menu_desc_t main_menu;
menu_desc_t settings_menu = {"settings", 0, &main_settings_items, &main_menu};

void setupSettingsPage()
{
    menu_item_t item_1 = {nameItems[0], NULL, NULL, NULL};
    cvector_push_back(main_settings_items, item_1);
    menu_item_t item_2 = {nameItems[1], NULL, NULL, NULL};
    cvector_push_back(main_settings_items, item_2);
    menu_item_t item_3 = {nameItems[2], NULL, NULL, NULL};
    cvector_push_back(main_settings_items, item_3);
    menu_item_t item_4 = {nameItems[3], NULL, NULL, NULL};
    cvector_push_back(main_settings_items, item_4);
    menu_item_t item_5 = {nameItems[4], NULL, NULL, NULL};
    cvector_push_back(main_settings_items, item_5);
    menu_item_t item_6 = {nameItems[5], NULL, NULL, NULL};
    cvector_push_back(main_settings_items, item_6);
    menu_item_t item_7 = {nameItems[6], NULL, NULL, NULL};
    cvector_push_back(main_settings_items, item_7);
}