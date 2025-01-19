#include "display/disp_task.h"
#include "display/image_start_logo.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <sharpdisp/bitmapimage.h>
#include <fonts/liberation_sans_24.h>
#include <uiElements/ui_menu.h>
#include "button.h"

void (*drawFunction)(void);
TaskHandle_t displayHandle;

extern void setupMainPage();
extern void setupSettingsPage();

static struct SharpDisp sd;
struct BitmapText text_24;
struct BitmapImages start_image;
static const char *status_line = NULL;
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
    text_24.x = 0;
    text_24.y = 0;

    text_str(&text_24, status_line);
    xSemaphoreGive(dispSem);
}

void drawStatusStr(const char *str)
{
    status_line = str;
}

static void show_image(uint32_t idx, struct BitmapImages *bitmap)
{
    int16_t width = image_width(bitmap, idx);
    int16_t height = image_height(bitmap, idx);
    int16_t x = (WIDTH - width) / 2;
    int16_t y = (HEIGHT - height) / 2;
    bitmap_clear(&sd.bitmap);
    image_draw(bitmap, idx, x, y);
}

void display_task(__unused void *params)
{
    const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(50);
    dispSem = xSemaphoreCreateMutex();
    xSemaphoreGive(dispSem);

    sharpdisp_init_default(&sd, disp_buffer_1, WIDTH, HEIGHT, 0xFF);
    text_init(&text_24, liberation_sans_24, &sd.bitmap);
    image_init(&start_image, image_start_logo, &sd.bitmap);
    bitmap_clear(&sd.bitmap);

    show_image(0, &start_image);
    text_24.x = (WIDTH - text_str_width(&text_24, "SPECTER"))/2;
    text_24.y = 207;
    text_str(&text_24, "SPECTER");

    const uint32_t steps = 5000 / 50;
    for (uint32_t i = 0; i < steps; ++i)
    {
        sharpdisp_refresh(&sd);
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    setupMainPage();
    setupSettingsPage();

    while (true)
    {
        xSemaphoreTake(dispSem, portMAX_DELAY);
        taskENTER_CRITICAL();
        sharpdisp_refresh(&sd);
        taskEXIT_CRITICAL();
        xSemaphoreGive(dispSem);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
