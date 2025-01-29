#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <uiElements/ui_menu.h>

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

#endif