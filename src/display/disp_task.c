#include "display/disp_task.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <sharpdisp/sharpdisp.h>
#include <uiElements/ui_menu.h>
#include "button.h"

void (*drawFunction)(void);
TaskHandle_t displayHandle;

extern void (*actionBack)(void);

static struct SharpDisp sd;
static SemaphoreHandle_t dispSem;
static uint8_t disp_buffer_1[BITMAP_SIZE(WIDTH, HEIGHT)];

/// @brief 
/// call for update information on screen
void drawDisplay()
{
    xSemaphoreTake(dispSem, portMAX_DELAY);
    if (drawFunction)
        drawFunction();
    xSemaphoreGive(dispSem);
    // xTaskNotifyGive(displayHandle);
}

void display_task(__unused void *params)
{
    const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(50);
    dispSem = xSemaphoreCreateMutex();
    xSemaphoreGive(dispSem);

    sharpdisp_init_default(&sd, disp_buffer_1, WIDTH, HEIGHT, 0xFF);
    bitmap_clear(&sd.bitmap);
    
    // setup first menu
    drawFunction = &drawMenu;
    actionBack = &actionButtonBack;
    setButtonHandler(1, actionButtonDown);
    setButtonHandler(2, actionButtonUP);
    setButtonHandler(3, actionButtonSelect);
    //
    
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
