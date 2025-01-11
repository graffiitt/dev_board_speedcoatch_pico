#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

#include "hardware/structs/rosc.h"

#include "display/disp_task.h"
#include "button.h"

// Which core to run on if configNUMBER_OF_CORES==1
#ifndef RUN_FREE_RTOS_ON_CORE
#define RUN_FREE_RTOS_ON_CORE 0
#endif

// Delay between led blinking
#define LED_DELAY_MS 21

// Priorities of our threads - higher numbers are higher priority
#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 20UL)
#define BUTTON_TASK_PRIORITY (tskIDLE_PRIORITY + 19UL)
#define DISPLAY_TASK_PRIORITY (tskIDLE_PRIORITY + 19UL)

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

void vLaunch(void)
{
    TaskHandle_t task;
    setButtonHandler(0, actionBackButton);

    xTaskCreate(display_task, "displayDraw", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, &displayHandle);
    xTaskCreate(buttonTask, "buttonHandler", BUTTON_TASK_STACK_SIZE, NULL, BUTTON_TASK_PRIORITY, NULL);

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
