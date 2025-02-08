#include "display/page.h"
#include "button.h"
#include "FreeRTOS.h"
#include "task.h"

extern const char *const itemNameStr[];
static const char strName[] = "view elements data";
static uint8_t *elements;
static int8_t currentItem = 0;

static void paintLine(const int x, const int y, const char *str)
{
    getText_24()->x = x;
    getText_24()->y = y;
    text_str(getText_24(), str);
}

static void painter()
{
    char str[20];

    sprintf(str, "1: %s", itemNameStr[elements[0]]);
    paintLine(25, 37, str);
    sprintf(str, "2: %s", itemNameStr[elements[1]]);
    paintLine(25, 68, str);
    sprintf(str, "3: %s", itemNameStr[elements[2]]);
    paintLine(25, 99, str);
    sprintf(str, "4: %s", itemNameStr[elements[3]]);
    paintLine(25, 130, str);

    getText_24()->y = 37 + (text_height(getText_24()) + 7) * currentItem;
    getText_24()->x = 0;
    text_char(getText_24(), '>');
}

static void buttonSelect(enum BUTTON_ACTION act)
{
    if (act != SHORT)
        return;
    drawDisplay();
    bitmap_hline(getBitmap(), 0, 240 - 26, WIDTH);
    getText_24()->x = 0;
    getText_24()->y = 240 - 25;
    text_str(getText_24(), "saving");

    flash_write_in_Page(0, 8, elements, 4);

    vTaskDelay(2000);
    drawDisplay();
}

static void buttonUp(enum BUTTON_ACTION act)
{
    switch (act)
    {
    case HOLDING:
        if (currentItem < 3)
            currentItem++;
        else
            currentItem = 0;
        break;
    case SHORT:
        if (elements[currentItem] < NUM_ITEMS - 1)
            elements[currentItem]++;
        else
            elements[currentItem] = 0;
        break;
    }
    drawDisplay();
}

static void buttonDown(enum BUTTON_ACTION act)
{
    switch (act)
    {
    case HOLDING:
        if (currentItem > 0)
            currentItem--;
        else
            currentItem = 3;
        break;
    case SHORT:
        if (elements[currentItem] > 0)
            elements[currentItem]--;
        else
            elements[currentItem] = NUM_ITEMS - 1;
        break;
    }
    drawDisplay();
}

static void exitScreen()
{
    free(elements);
    drawFunction = &drawMenu;
    setupCallbacksMenu();
    setMenu(&settings_menu);
}

void setupViewDataPage(enum MENU_ACTIONS action)
{
    if (action != MENU_ACTIONS_RUN)
        return;
    elements = malloc(4);
    flash_read_data(8, elements, 4);

    drawStatusStr(strName);
    drawFunction = &painter;
    actionBack = exitScreen;
    setButtonHandler(1, buttonDown);
    setButtonHandler(2, buttonUp);
    setButtonHandler(3, buttonSelect);
    drawDisplay();
}
