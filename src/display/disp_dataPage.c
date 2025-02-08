#include "trainTask/train.h"
#include "display/page.h"
#include "button.h"
#include "hardware/rtc.h"
#include "FreeRTOS.h"
#include "task.h"

typedef void (*itemDrawFunc)(const uint16_t, const uint16_t);
typedef struct
{
    const char *name;
    itemDrawFunc draw;
} ItemData_t;

static void drawPulseItem(const uint16_t x, const uint16_t y);
static void drawStrokeCountItem(const uint16_t x, const uint16_t y);
static void drawStroceRateItem(const uint16_t x, const uint16_t y);
static void drawSplitItem(const uint16_t x, const uint16_t y);
static void drawDistanse(const uint16_t x, const uint16_t y);
static void drawChrono(const uint16_t x, const uint16_t y);

static uint8_t *currentItems;
const char * const itemNameStr[] = {"pulse", "stroke count", "stroke rate", "split", "distance", "chrono"};
static ItemData_t dataItem[] = {{itemNameStr[0], drawPulseItem},
                                {itemNameStr[1], drawStrokeCountItem},
                                {itemNameStr[2], drawStroceRateItem},
                                {itemNameStr[3], drawSplitItem},
                                {itemNameStr[4], drawDistanse},
                                {itemNameStr[5], drawChrono}};

static char str[12] = {0};
TaskHandle_t trainTask_h;

static void painter()
{
    datetime_t t;
    if (rtc_get_datetime(&t))
    {
        sprintf(str, "%02d:%02d:%02d", t.hour, t.min, t.sec);
    }

    dataItem[currentItems[0]].draw(0, 27);
    dataItem[currentItems[1]].draw(0, 134);
    dataItem[currentItems[2]].draw(201, 27);
    dataItem[currentItems[3]].draw(201, 134);

    bitmap_hline(getBitmap(), 0, 133, WIDTH);
    bitmap_vline(getBitmap(), 200, 26, HEIGHT - 26);
}

static void stopTrain()
{
    free(currentItems);
    vTaskDelete(trainTask_h);
    drawFunction = &drawMenu;
    setupCallbacksMenu();
    setMenu(&main_menu);
}

void dataDisplay(enum MENU_ACTIONS action)
{
    if (action != MENU_ACTIONS_RUN)
        return;
    currentItems = malloc(4);
    flash_read_data(8, currentItems, 4);

    drawFunction = &painter;
    actionBack = stopTrain;
    setButtonHandler(1, NULL);
    setButtonHandler(2, NULL);
    setButtonHandler(3, NULL);
    drawStatusStr(str);

    drawDisplay();
    xTaskCreate(trainTask, "train", configMINIMAL_STACK_SIZE, NULL, 20, &trainTask_h);
}

void drawPulseItem(const uint16_t x, const uint16_t y)
{
    uint8_t pulse = 123;

    getText_80()->x = x + 16;
    getText_80()->y = y + 16;
    char str[3];
    sprintf(str, "%03d", pulse);
    text_str(getText_80(), str);

    drawStatusImage(3, x + 166, y + 16);
}

void drawStrokeCountItem(const uint16_t x, const uint16_t y)
{
}

void drawStroceRateItem(const uint16_t x, const uint16_t y)
{
}

void drawSplitItem(const uint16_t x, const uint16_t y)
{
}

void drawDistanse(const uint16_t x, const uint16_t y)
{
}

void drawChrono(const uint16_t x, const uint16_t y)
{
}
