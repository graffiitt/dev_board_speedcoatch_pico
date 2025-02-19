#include <sharpdisp/sharpdisp.h>
#include <display/page.h>
#include "button.h"

static char *nameItems[] = {"bluetooth", "view elements", "gps info", "time setup", "item2", "about"};

static cvector(menu_item_t) main_settings_items = NULL;
menu_desc_t settings_menu = {"settings", 0, &main_settings_items, &main_menu};

void setupSettingsPage()
{
    menu_item_t item_1 = {nameItems[0], str_state[1], &bluetoothAction, NULL};
    cvector_push_back(main_settings_items, item_1);
    menu_item_t item_2 = {nameItems[1], 0, &setupViewDataPage, NULL};
    cvector_push_back(main_settings_items, item_2);
    menu_item_t item_3 = {nameItems[2], 0, NULL, NULL};
    cvector_push_back(main_settings_items, item_3);
    menu_item_t item_4 = {nameItems[3], 0, &watchSettingsDisplay, NULL};
    cvector_push_back(main_settings_items, item_4);
    menu_item_t item_5 = {nameItems[4], 0, NULL, NULL};
    cvector_push_back(main_settings_items, item_5);
    menu_item_t item_6 = {nameItems[5], 0, NULL, NULL};
    cvector_push_back(main_settings_items, item_6);
}
