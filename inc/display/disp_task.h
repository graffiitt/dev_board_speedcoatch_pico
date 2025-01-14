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
void drawDisplay();
void drawStatusStr(const char* str);
void display_task(__unused void *params);

#endif