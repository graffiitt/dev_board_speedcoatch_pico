#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

#include <sharpdisp/sharpdisp.h>
#include <sharpdisp/bitmapimage.h>
#include <sharpdisp/bitmapshapes.h>
#include <sharpdisp/bitmaptext.h>
#include <fonts/liberation_sans_36.h>
#include "images.h"

#include "hardware/structs/rosc.h"

// Which core to run on if configNUMBER_OF_CORES==1
#ifndef RUN_FREE_RTOS_ON_CORE
#define RUN_FREE_RTOS_ON_CORE 0
#endif

// Delay between led blinking
#define LED_DELAY_MS 21

// Priorities of our threads - higher numbers are higher priority
#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 2UL)
#define BLINK_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define DISPLAY_TASK_PRIORITY (tskIDLE_PRIORITY + 4UL)

// Stack sizes of our threads in words (4 bytes)
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define BLINK_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define DISPLAY_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#define WIDTH 400
#define HEIGHT 240

#define FRAME_MS 16
uint8_t disp_buffer[BITMAP_SIZE(WIDTH, HEIGHT)];

void blink_task(__unused void *params)
{
    bool on = false;
    while (true)
    {
        vTaskDelay(LED_DELAY_MS);
    }
}

int count = 0;
TaskHandle_t taskDisplay;
void main_task(__unused void *params)
{
    while (true)
    {
        count++;
        vTaskDelay(pdMS_TO_TICKS(500));
        xTaskNotifyGive(taskDisplay);
    }
}

struct SharpDisp sd;
struct BitmapImages bi;
struct BitmapText text;

static int16_t rand16(int16_t min, int16_t max)
{
    uint16_t v = 0x0000;
    for (int i = 0; i < 16; ++i, v <<= 1)
    {
        if (rosc_hw->randombit)
        {
            v |= 0x0001;
        }
    }
    return min + (v % (max - min));
}
uint32_t t = 0;

static void show_an_image(uint32_t idx)
{
    int16_t width = image_width(&bi, idx);
    int16_t height = image_height(&bi, idx);
    int16_t x = (WIDTH - width) / 2;
    int16_t y = (HEIGHT - height) / 2;
    if (rand16(0, 100) > 50)
    {
        sd.bitmap.clear_byte = 0xFF;
    }
    else
    {
        sd.bitmap.clear_byte = 0x00;
    }
    bitmap_clear(&sd.bitmap);
    image_draw(&bi, idx, x, y);
}

static void sleep_for(uint32_t ms)
{
    const uint32_t steps = ms / 50;
    for (uint32_t i = 0; i < steps; ++i)
    {
        sharpdisp_refresh(&sd);
        vTaskDelay(50);
    }
}

void display_task(__unused void *params)
{
    const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(2000);

    sharpdisp_init_default(&sd, disp_buffer, WIDTH, HEIGHT, 0xFF);
    image_init(&bi, images, &sd.bitmap);
    text_init(&text, liberation_sans_36, &sd.bitmap);
    sd.bitmap.mode = BITMAP_INVERSE;
    const uint32_t num_images = image_count(&bi);
    while (true)
    {
        for (uint32_t idx = 0; idx < num_images; ++idx)
        {
            show_an_image(idx);
            char str[5];
            sprintf(str, "%d", idx);
            // bitmap_clear(&sd.bitmap);
            text.x = 0;
            text.y = 0;
            text_str(&text, str);
            sleep_for(1000);
        }
    }
}

void vLaunch(void)
{
    TaskHandle_t task;
    xTaskCreate(main_task, "MainThread", MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &task);
    // start the led blinking
    xTaskCreate(blink_task, "BlinkThread", BLINK_TASK_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, NULL);
    xTaskCreate(display_task, "DisplThread", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, &taskDisplay);

#if configUSE_CORE_AFFINITY && configNUMBER_OF_CORES > 1
    // we must bind the main task to one core (well at least while the init is called)
    vTaskCoreAffinitySet(task, 1);
#endif

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main(void)
{
    timer_hw->dbgpause = 0x2;

    vLaunch();
    return 0;
}
