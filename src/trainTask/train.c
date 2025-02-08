#include "trainTask/train.h"
#include "FreeRTOS.h"
#include "task.h"

void trainTask(__unused void *params)
{

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
       // drawDisplay();
    }
}