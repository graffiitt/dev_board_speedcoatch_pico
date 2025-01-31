#ifndef MAINPAGE_H
#define MAINPAGE_H

#define NUM_ITEMS 6

#include <uiElements/ui_menu.h>
#include "w25qxx/w25qxx.h"

void drawStatusImage(int id, uint16_t x, uint16_t y);

extern menu_desc_t settings_menu;
extern menu_desc_t main_menu;
extern menu_desc_t bluetooth_menu;
 
extern char *str_state[];
extern void bluetoothAction(enum MENU_ACTIONS action);

extern void (*actionBack)(void);
extern void watchSettingsDisplay(enum MENU_ACTIONS);
extern void setupMainPage();
extern void setupSettingsPage();
extern void setupWatchSettingsDisplay();
extern void dataDisplay(enum MENU_ACTIONS action);
extern void setupViewDataPage(enum MENU_ACTIONS action);

#endif