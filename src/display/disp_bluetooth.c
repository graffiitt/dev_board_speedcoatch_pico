#include "display/page.h"

bool current_state_ble = false;
char *str_state[] = {"on", "off"};

static cvector(menu_item_t) ble_items = NULL;

menu_desc_t bluetooth_menu = {"bluetooth", 0, &ble_items, &settings_menu};

void ble_on()
{
    menu_item_t *item = cvector_at(*settings_menu.items, 0);
    item->descrItem = str_state[0];
    current_state_ble = true;
    drawDisplay();
}

void ble_off()
{
    current_state_ble = false;
    menu_item_t *item = cvector_at(*settings_menu.items, 0);
    item->descrItem = str_state[1];
    drawDisplay();
}

void bluetoothAction(enum MENU_ACTIONS action)
{
    if (action != MENU_ACTIONS_DOWN)
        return;

    current_state_ble ? ble_off() : ble_on();
}
