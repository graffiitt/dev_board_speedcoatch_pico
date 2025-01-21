#include "uiElements/ui_spinbox_screen.h"
#include "display/page.h"
#include "button.h"
#include "hardware/rtc.h"

static cvector(spinbox_int) items = NULL;
static spinbox_screen_t screen = {"time setup", &items, 0, false};

static void exitScreen()
{
    if (screen.save)
    {
        datetime_t t;
        rtc_get_datetime(&t);
        t.hour = cvector_at(*screen.items, 0)->data;
        t.min = cvector_at(*screen.items, 1)->data;
        t.day = cvector_at(*screen.items, 2)->data;
        t.month = cvector_at(*screen.items, 3)->data;
        t.year = cvector_at(*screen.items, 4)->data;
        rtc_set_datetime(&t);
    }
    drawFunction = &drawMenu;
    setupCallbacksMenu();
    setMenu(&settings_menu);
}

void setupWatchSettingsDisplay()
{
    spinbox_int item1 = {25, 37, "hour", 12, 0, 24};
    cvector_push_back(items, item1);
    spinbox_int item2 = {200, 37, "min", 0, 0, 59};
    cvector_push_back(items, item2);
    spinbox_int item3 = {25, 68, "day", 1, 1, 31};
    cvector_push_back(items, item3);
    spinbox_int item4 = {25, 99, "month", 1, 1, 12};
    cvector_push_back(items, item4);
    spinbox_int item5 = {25, 130, "year", 2025, 2020, 3000};
    cvector_push_back(items, item5);
}

void watchSettingsDisplay(enum MENU_ACTIONS action)
{
    if (action != MENU_ACTIONS_RUN)
        return;
    datetime_t t;
    if (!rtc_get_datetime(&t))
        return;

    actionBack = &exitScreen;

    cvector_at(*screen.items, 0)->data = t.hour;
    cvector_at(*screen.items, 1)->data = t.min;
    cvector_at(*screen.items, 2)->data = t.day;
    cvector_at(*screen.items, 3)->data = t.month;
    cvector_at(*screen.items, 4)->data = t.year;

    setSpinBoxScreen(&screen);
}
