#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "FreeRTOS.h"
#include "task.h"

#include "hardware/rtc.h"

#include "display/disp_task.h"
#include "display/page.h"
#include "button.h"
#include "w25qxx/w25qxx.h"
#include "bluetooth/bluetooth_core.h"
#include "mpu.h"
#include "gps.h"
#include "battery.h"

// Priorities of our threads - higher numbers are higher priority
#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 20UL)
#define BUTTON_TASK_PRIORITY (tskIDLE_PRIORITY + 21UL)
#define DISPLAY_TASK_PRIORITY (tskIDLE_PRIORITY + 21UL)

// Stack sizes of our threads in words (4 bytes)
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define BUTTON_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define DISPLAY_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

TaskHandle_t buttonHandle;
TaskHandle_t displayHandle;

static bool powerState = true;
bool maySleep = true;

void (*actionBack)(void);
extern void ble_off_action();

static void startTasks()
{
    xTaskCreate(buttonTask, "buttonHandler", BUTTON_TASK_STACK_SIZE, NULL, BUTTON_TASK_PRIORITY, &buttonHandle);
    xTaskCreate(display_task, "displayDraw", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, &displayHandle);
    vTaskCoreAffinitySet(displayHandle, 2);
}

void actionPower()
{
    if (!maySleep)
        return;

    actionBack = NULL;
    setButtonHandler(1, NULL);
    setButtonHandler(2, NULL);
    setButtonHandler(3, NULL);

    if (powerState)
    {
        powerState = false;

        ble_off_action();
        gps_off();
        flash_enter_power_down();
        vTaskDelete(displayHandle);
        vTaskDelete(buttonHandle);
    }
    else
    {
        powerState = true;
        flash_exit_power_down();
        gps_on();
        startTasks();
    }
}

void actionBackButton(enum BUTTON_ACTION act)
{
    switch (act)
    {
    case SHORT:
        if (actionBack)
            actionBack();
        break;
    case HOLDING:
        actionPower();
        break;
    }
}

static char taskList[500] = {0};
void mainTask(__unused void *params)
{
    ble_init();
    mpu_init();
    startTasks();
    initGps();
    setupMainPage();
    setupSettingsPage();
    setupWatchSettingsDisplay();
    vTaskDelay(pdMS_TO_TICKS(5000));

    while (1)
    {
        if (powerState)
        {

            // printf("task states\n");
            // vTaskList(taskList);
            // printf("%s\n", taskList);
            vTaskDelay(pdMS_TO_TICKS(1000));
            drawChargeState(adc_getImageCharge());
        }
        else
        {
            static button_cfg_t bt;

            int8_t state = gpio_get(BUTTON_1);
            if ((!state) && bt.last_state) // fix pressed
            {
                bt.timePress = time_us_32();
                bt.flag = 1;
            }

            if (!(state && bt.last_state) &&
                ((time_us_32() - bt.timePress) > TIME_PRESS_LONG) && bt.flag)
            {
                bt.flag = 0;
                actionPower();
            }
            bt.last_state = state;
            vTaskDelay(UPDATE_BUTTONS_TIME);
        }
    }
}

void vLaunch(void)
{
    xTaskHandle mainTsk;
    setButtonHandler(0, actionBackButton);

    xTaskCreate(mainTask, "mainTask", 526, NULL, tskIDLE_PRIORITY, &mainTsk);
    // we must bind the main task to one core (well at least while the init is called)
    vTaskCoreAffinitySet(mainTsk, 1);

    vTaskStartScheduler();
}

int main(void)
{
    timer_hw->dbgpause = 0x2 | 0x4;

    stdio_init_all();
    setup_adc();
    initSPI();
    rtc_init();
    // datetime_t t = {
    //     .year = 2025,
    //     .month = 01,
    //     .day = 020,
    //     .dotw = 1, // 0 is Sunday, so 5 is Friday
    //     .hour = 15,
    //     .min = 45,
    //     .sec = 00};
    // rtc_set_datetime(&t);

    vLaunch();
    return 0;
}
