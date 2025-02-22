#include "display/disp_task.h"
#include "display/page.h"
#include "display/status_line_images.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <sharpdisp/bitmapimage.h>
#include <fonts/liberation_sans_24.h>
#include <fonts/liberation_sans_80.h>

#include <uiElements/ui_menu.h>
#include "button.h"

void (*drawFunction)(void);
TaskHandle_t displayHandle;

static struct SharpDisp sd;
static struct BitmapText text_24;
static struct BitmapText text_80;
static struct BitmapImages status_images;
static const char *status_line = NULL;
static uint8_t disp_buffer_1[BITMAP_SIZE(WIDTH, HEIGHT)];
static SemaphoreHandle_t dispSem;

extern bool state_ble;
static int8_t chargeBattImage = BATTERY_10_IMG;

struct Bitmap *getBitmap()
{
    return &sd.bitmap;
}

struct BitmapText *getText_24()
{
    return &text_24;
}

struct BitmapText *getText_80()
{
    return &text_80;
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

    if (state_ble)
    {
        image_draw(&status_images, BLUETOOTH_IMG, 346, 0);
    }
    image_draw(&status_images, chargeBattImage, 370, 0);

    xSemaphoreGive(dispSem);
}

void drawStatusStr(const char *str)
{
    status_line = str;
}

void drawChargeState(int8_t indexImg)
{
    chargeBattImage = indexImg;
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

void showStartScreen()
{
    const char str_logo[] = "SPECTER";
    struct BitmapImages start_image;
    uint8_t *image = malloc(1390);

    flash_read_data(12800000, image, 1390);
    image_init(&start_image, image, &sd.bitmap);
    show_image(0, &start_image);
    free(image);

    text_24.x = (WIDTH - text_str_width(&text_24, str_logo)) / 2;
    text_24.y = 207;
    text_str(&text_24, str_logo);

    const uint32_t steps = 2500 / 50;
    for (uint32_t i = 0; i < steps; ++i)
    {
        sharpdisp_refresh(&sd);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void display_task(__unused void *params)
{
    const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(50);
    dispSem = xSemaphoreCreateMutex();
    xSemaphoreGive(dispSem);

    sharpdisp_init_default(&sd, disp_buffer_1, WIDTH, HEIGHT, 0xFF);
    text_init(&text_24, liberation_sans_24, &sd.bitmap);
    text_init(&text_80, liberation_sans_80, &sd.bitmap);
    image_init(&status_images, status_line_images, &sd.bitmap);
    bitmap_clear(&sd.bitmap);

    showStartScreen();

    drawFunction = &drawMenu;
    setupCallbacksMenu();
    setMenu(&main_menu);
    
    int8_t counter = 0;
    while (true)
    {
        if (counter > 2)
        {
            drawDisplay();
            counter = 0;
        }
        counter++;

        xSemaphoreTake(dispSem, portMAX_DELAY);
        taskENTER_CRITICAL();
        sharpdisp_refresh(&sd);
        taskEXIT_CRITICAL();
        xSemaphoreGive(dispSem);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void drawStatusImage(int id, uint16_t x, uint16_t y)
{
    image_draw(&status_images, id, x, y);
}