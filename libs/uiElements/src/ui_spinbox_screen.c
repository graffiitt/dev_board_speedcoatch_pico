#include "uiElements/ui_spinbox_screen.h"
#include "button.h"

static spinbox_screen_t *current_screen = NULL;

static void painter()
{
    for (uint8_t i = 0; i < cvector_size(*current_screen->items); i++)
    {
        drawSpinBox(cvector_at(*current_screen->items, i), current_screen->current_item == i);
    }
    if (current_screen->save)
    {
        bitmap_hline(getBitmap(), 0, 240 - 26, WIDTH);
        getText_24()->x = 0;
        getText_24()->y = 240 - 25;
        text_str(getText_24(), "saved");
    }
}

static void upCallback(enum BUTTON_ACTION act)
{
    switch (act)
    {
    case PUSH:
        break;
    case SHORT:
        incrementSpinBox(cvector_at(*current_screen->items, current_screen->current_item));
        drawDisplay();
        break;
    }
}

static void downCallback(enum BUTTON_ACTION act)
{
    switch (act)
    {
    case PUSH:
        break;
    case SHORT:
        decrementSpinBox(cvector_at(*current_screen->items, current_screen->current_item));
        drawDisplay();
        break;
    }
}

static void selectCallback(enum BUTTON_ACTION act)
{
    switch (act)
    {
    case HOLDING:
        current_screen->save = true;
        break;
    case SHORT:
        if ((cvector_size(*current_screen->items) - 1) > current_screen->current_item && cvector_size(*current_screen->items) != 0)
            current_screen->current_item++;
        else
            current_screen->current_item = 0;
        break;
    }
    printf("%d %d\n", cvector_size(*current_screen->items), current_screen->current_item);
    drawDisplay();
}

void setSpinBoxScreen(spinbox_screen_t *screen)
{
    if (screen == NULL)
        return;

    current_screen = screen;
    current_screen->save = false;
    drawStatusStr(current_screen->nameItem);
    drawFunction = &painter;
    setButtonHandler(1, downCallback);
    setButtonHandler(2, upCallback);
    setButtonHandler(3, selectCallback);

    drawDisplay();
}
