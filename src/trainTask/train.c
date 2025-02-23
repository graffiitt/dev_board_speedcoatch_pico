#include "trainTask/train.h"
#include "FreeRTOS.h"
#include "task.h"
#include "display/page.h"
#include <sharpdisp/bitmapimage.h>

uint8_t *image;

static void paintImage()
{
    const char str_logo[] = "starting train";
    struct BitmapImages start_image;
    image = malloc(600);

    flash_read_data(12801536, image, 600);
    image_init(&start_image, image, getBitmap());
    // show_image(0, &start_image);
    image_draw(&start_image, 0, 125, 45);
    free(image);

    getText_24()->x = (WIDTH - text_str_width(getText_24(), str_logo)) / 2;
    getText_24()->y = 207;
    text_str(getText_24(), str_logo);
}

void trainTask(void *params)
{
    drawFunction = paintImage;
    vTaskDelay(5000);
    drawFunction = params;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        // drawDisplay();
    }
}

void endTrain()
{
    
}