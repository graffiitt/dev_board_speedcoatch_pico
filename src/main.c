#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "FreeRTOS.h"
#include "task.h"

#include "hardware/structs/rosc.h"

#include "display/disp_task.h"
#include "button.h"


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

void mainTask()
{
    while (1)
    {

    }   
};

void vLaunch(void)
{
    TaskHandle_t task;
    TaskHandle_t taskw;
    
    setButtonHandler(0, actionBackButton);


    xTaskCreate(mainTask, "mainTask", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, NULL);
    xTaskCreate(display_task, "displayDraw", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, &displayHandle);
    xTaskCreate(buttonTask, "buttonHandler", BUTTON_TASK_STACK_SIZE, NULL, BUTTON_TASK_PRIORITY, NULL);

    // we must bind the main task to one core (well at least while the init is called)
    vTaskCoreAffinitySet(displayHandle, 1);

    vTaskStartScheduler();
}

int main(void)
{
    timer_hw->dbgpause = 0x2;
    
    // stdio_init_all();

    vLaunch();
    return 0;
}
