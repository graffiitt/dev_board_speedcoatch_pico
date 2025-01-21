#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "FreeRTOS.h"
#include "task.h"

#include "hardware/rtc.h"

#include "display/disp_task.h"
#include "button.h"

// Delay between led blinking
#define LED_DELAY_MS 21

// Priorities of our threads - higher numbers are higher priority
#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 20UL)
#define BUTTON_TASK_PRIORITY (tskIDLE_PRIORITY + 21UL)
#define DISPLAY_TASK_PRIORITY (tskIDLE_PRIORITY + 21UL)

// Stack sizes of our threads in words (4 bytes)
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define BUTTON_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define DISPLAY_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

extern TaskHandle_t displayHandle;

void (*actionBack)(void);
void actionPower()
{
}

void actionBackButton(enum BUTTON_ACTION act)
{
    switch (act)
    {
    case SHORT:
        if (actionBack)
            actionBack();
    case HOLDING:
        actionPower();
    }
}

static char taskList[500] = {0};
void mainTask(__unused void *params)
{
    xTaskCreate(buttonTask, "buttonHandler", BUTTON_TASK_STACK_SIZE, NULL, BUTTON_TASK_PRIORITY, NULL);
        vTaskDelay(pdMS_TO_TICKS(5000));

    while (1)
    {
        printf("task states\n");
        vTaskList(taskList);
        printf("%s\n", taskList);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
};

void vLaunch(void)
{
    xTaskHandle mainTsk;
    setButtonHandler(0, actionBackButton);

    xTaskCreate(mainTask, "mainTask", 526, NULL, tskIDLE_PRIORITY, &mainTsk);
    xTaskCreate(display_task, "displayDraw", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, &displayHandle);
    // we must bind the main task to one core (well at least while the init is called)
    vTaskCoreAffinitySet(mainTsk, 1);
    vTaskCoreAffinitySet(displayHandle, 2);

    vTaskStartScheduler();
}

int main(void)
{
    timer_hw->dbgpause = 0x2;

    stdio_init_all();

    rtc_init();
    datetime_t t = {
        .year = 2025,
        .month = 01,
        .day = 020,
        .dotw = 1, // 0 is Sunday, so 5 is Friday
        .hour = 15,
        .min = 45,
        .sec = 00};
    rtc_set_datetime(&t);

    vLaunch();
    return 0;
}
