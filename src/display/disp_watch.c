#include <sharpdisp/sharpdisp.h>
#include "display/disp_watch.h"
#include "button.h"
#include "hardware/rtc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mpu.h"

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
        char str[40] = {0};

        getText_80()->x = 30;
        getText_80()->y = 57;
        sprintf(str, "%02d:%02d:%02d", t.hour, t.min, t.sec);
        text_str(getText_80(), str);

        getText_24()->x = 30;
        getText_24()->y = 147;
        sprintf(str, "%2d.%2d", t.day, t.month);
        text_str(getText_24(), str);

        getText_24()->x = 120;
        getText_24()->y = 147;
        sprintf(str, "%.1f", (mpu_getTemp()/340.0)+36.53);
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
    mpu_sleep(false);
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
    mpu_sleep(true);
    setupCallbacksMenu();
    setMenu(&main_menu);
}
