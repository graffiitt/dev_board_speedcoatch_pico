#include <sharpdisp/sharpdisp.h>
#include <uiElements/ui_menu.h>
#include "display/page.h"
#include "button.h"
#include "hardware/rtc.h"
#include "FreeRTOS.h"
#include "task.h"

static char *days[] = {"Sunday", "Monday", "Tuesday", "Wensday", "Thursday", "Friday", "Saturday"};

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
        getText_24()->x=0;
        getText_24()->y=137;
        sprintf(str, "%2d.%2d \t%s", t.day, t.month, days[t.dotw]);
        text_str(getText_24(), str);
    }
}

static bool stateTask = true; 
static void watchTask()
{

    while (stateTask)
    {
        drawDisplay();
        vTaskDelay(100);
    }
    vTaskDelete(NULL);
}

void watchDisplay()
{
    drawFunction = &painter;
    actionBack = watchStop;
    setButtonHandler(1, NULL);
    setButtonHandler(2, NULL);
    setButtonHandler(3, NULL);
    drawStatusStr(NULL);
    stateTask = true;
    xTaskCreate(watchTask, "watchTask", 120, NULL, 10, NULL);
}

void watchStop()
{
    // vTaskDelete(task);
    stateTask = false;

    drawFunction = &drawMenu;
    setupCallbacksMenu();
    setMenu(&main_menu);
}
