#include <sharpdisp/sharpdisp.h>
#include "display/disp_watch.h"
#include "button.h"
#include "hardware/rtc.h"
#include "FreeRTOS.h"
#include "task.h"

#include "uiElements/ui_spinbox_int.h"

// static bool stateTask = true;
static void watchStop();

static void painter()
{
    datetime_t t;
    if (!rtc_get_datetime(&t))
    {
        getText_24()->x = 0;
        getText_24()->y = 27;
        text_str(getText_24(), "rtc not running");
    }
    else
    {
        getText_80()->x = 0;
        getText_80()->y = 57;
        char str[40] = {0};
        sprintf(str, "%02d:%02d:%02d", t.hour, t.min, t.sec);

        text_str(getText_80(), str);
        getText_24()->x = 0;
        getText_24()->y = 137;
        sprintf(str, "%2d.%2d", t.day, t.month);
        text_str(getText_24(), str);
    }
}

// static void watchTask()
// {

//     while (stateTask)
//     {
//         drawDisplay();
//         vTaskDelay(100);
//     }
//     vTaskDelete(NULL);
// }

void watchDisplay(enum MENU_ACTIONS action)
{
    if (action != MENU_ACTIONS_RUN)
        return;

    drawFunction = &painter;
    actionBack = watchStop;
    setButtonHandler(1, NULL);
    setButtonHandler(2, NULL);
    setButtonHandler(3, NULL);
    drawStatusStr(NULL);
    drawDisplay();
    // stateTask = true;
   // xTaskCreate(watchTask, "watchTask", 140, NULL, 10, NULL);
}

void watchStop()
{
    // vTaskDelete(task);
    // stateTask = false;

    drawFunction = &drawMenu;
    setupCallbacksMenu();
    setMenu(&main_menu);
}
