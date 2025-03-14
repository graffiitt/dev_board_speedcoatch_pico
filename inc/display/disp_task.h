#ifndef DISP_TASK
#define DISP_TASK

#include <sharpdisp/sharpdisp.h>
#include <sharpdisp/bitmapshapes.h>
#include <sharpdisp/bitmaptext.h>
#include "pico/stdlib.h"

#define WIDTH 400
#define HEIGHT 240

extern void (*drawFunction)(void);

struct Bitmap *getBitmap();
struct BitmapText *getText_24();
struct BitmapText *getText_48();
struct BitmapText *getText_80();

void drawDisplay();
void drawStatusStr(const char *str);
void drawChargeState(int8_t indexImg);

void display_task(__unused void *params);
void showStartScreen();
#endif