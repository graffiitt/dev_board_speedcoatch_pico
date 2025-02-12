#include "display/page.h"
#include "bluetooth/bluetooth_core.h"
#include "button.h"

extern int listener_registered;

static ble_device_t curr_dev;
static int8_t current_dev = 0;
bool state_ble = false;
char *str_state[] = {"on", "off"};
static const char strName[] = "bluetooth";

static void ble_on_action()
{
    menu_item_t *item = cvector_at(*settings_menu.items, 0);
    item->descrItem = str_state[0];
    state_ble = true;
    ble_on();
}

static void ble_off_action()
{
    state_ble = false;
    menu_item_t *item = cvector_at(*settings_menu.items, 0);
    item->descrItem = str_state[1];
    ble_off();
}

static void painter()
{
    // coordinates for start draw menu;
    uint8_t x = 25;
    uint8_t y = 37;

    if (listener_registered == 1)
    {
        char str[20];
        getText_24()->x = x;
        getText_24()->y = y;
        text_str(getText_24(), curr_dev.name);
        getText_24()->x = x;
        getText_24()->y = y + 31;
        sprintf(str, "pulse: %03d cnt: %d", hrt.heart_rate, hrt.sensor_contact);
        text_str(getText_24(), str);
    }
    else
    {
        if (cvector_size(ble_devices) == 0)
            return;

        getText_24()->x = x;
        getText_24()->y = y;
        if (current_dev > cvector_size(ble_devices) - 1)
        {
            current_dev = cvector_size(ble_devices) - 1;
        }

        for (uint8_t i = 0; i < NUM_DISPLAY_ROWS; i++)
        {
            int item = current_dev > (NUM_DISPLAY_ROWS - 1) ? (current_dev - NUM_DISPLAY_ROWS + 1) + i : i;
            if (item < cvector_size(ble_devices))
                text_str(getText_24(), cvector_at(ble_devices, item)->name);
        }
        getText_24()->y = y + (text_height(getText_24()) + 7) * (current_dev > (NUM_DISPLAY_ROWS - 1) ? (NUM_DISPLAY_ROWS - 1) : current_dev);

        getText_24()->x = 0;
        text_char(getText_24(), '>');
    }
}

static void buttonSelect(enum BUTTON_ACTION act)
{
    if (act != SHORT)
        return;
    if (listener_registered == 1)
    { // disconnect
        ble_disconnect();
        ble_startScan();
    }
    else
    { // connect
        if (cvector_size(ble_devices) == 0)
            return;
        ble_stopScan();
        strcpy(curr_dev.name, cvector_at(ble_devices, current_dev)->name);
        bd_addr_copy(curr_dev.address, cvector_at(ble_devices, current_dev)->address);
        curr_dev.addr_type = cvector_at(ble_devices, current_dev)->addr_type;
        ble_clearDevices();

        ble_connect(curr_dev.address, curr_dev.addr_type);
    }
}

static void buttonUp(enum BUTTON_ACTION act)
{
    if (listener_registered == 1)
        return;

    switch (act)
    {
    case SHORT:
        if (current_dev > cvector_size(ble_devices) - 1)
        {
            current_dev = cvector_size(ble_devices) - 1;
        }
        else
        {
            current_dev++;
        }
        drawDisplay();
        break;
    }
}

static void buttonDown(enum BUTTON_ACTION act)
{
    if (listener_registered == 1)
        return;
    switch (act)
    {
    case SHORT:
        if (current_dev > 0)
            current_dev--;
        drawDisplay();
        break;
    }
}

static void exitScreen()
{
    if (listener_registered != 1)
    {
        ble_clearDevices();
        ble_stopScan();
    }

    drawFunction = &drawMenu;
    setupCallbacksMenu();
    setMenu(&settings_menu);
}

void bluetoothAction(enum MENU_ACTIONS action)
{
    switch (action)
    {
    case MENU_ACTIONS_SELECT_HOLD:
        state_ble ? ble_off_action() : ble_on_action();
        drawDisplay();
        break;

    case MENU_ACTIONS_RUN:
        if (state_ble == false)
            ble_on_action();
        drawStatusStr(strName);
        drawFunction = &painter;
        actionBack = exitScreen;
        setButtonHandler(1, buttonDown);
        setButtonHandler(2, buttonUp);
        setButtonHandler(3, buttonSelect);
        drawDisplay();

        if (listener_registered != 1)
            ble_startScan();
        break;
    }
}
