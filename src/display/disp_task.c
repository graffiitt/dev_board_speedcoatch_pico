#include "display/disp_task.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <fonts/liberation_sans_24.h>
#include <uiElements/ui_menu.h>
#include "button.h"

void (*drawFunction)(void);
TaskHandle_t displayHandle;

extern void setupMainPage();
extern void setupSettingsPage();

static struct SharpDisp sd;
struct BitmapText text_24;
static const char *statusStroke = NULL;
static uint8_t disp_buffer_1[BITMAP_SIZE(WIDTH, HEIGHT)];
static SemaphoreHandle_t dispSem;

struct Bitmap *getBitmap()
{
    return &sd.bitmap;
}

struct BitmapText *getText_24()
{
    return &text_24;
}

/// @brief
/// call for update information on screen
void drawDisplay()
{

    if (drawFunction == NULL)
        return;
    xSemaphoreTake(dispSem, portMAX_DELAY);
    bitmap_clear(&sd.bitmap);
    drawFunction();
    bitmap_hline(&sd.bitmap, 0, 26, WIDTH);
    text_24.x=0;
    text_24.y=0;

    text_str(&text_24, statusStroke);
    xSemaphoreGive(dispSem);

    // xTaskNotifyGive(displayHandle);
}

void drawStatusStr(const char *str)
{
    statusStroke = str;
}

void display_task(__unused void *params)
{
    const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(50);
    dispSem = xSemaphoreCreateMutex();
    xSemaphoreGive(dispSem);

    sharpdisp_init_default(&sd, disp_buffer_1, WIDTH, HEIGHT, 0xFF);
    text_init(&text_24, liberation_sans_24, &sd.bitmap);
    bitmap_clear(&sd.bitmap);

    setupMainPage();
    setupSettingsPage();

    while (true)
    {
        // ulTaskNotifyTake(pdTRUE, xMaxExpectedBlockTime);
        xSemaphoreTake(dispSem, portMAX_DELAY);
        taskENTER_CRITICAL();
        sharpdisp_refresh(&sd);
        taskEXIT_CRITICAL();
        xSemaphoreGive(dispSem);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
