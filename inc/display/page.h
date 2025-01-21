#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <uiElements/ui_menu.h>

extern menu_desc_t settings_menu;
extern menu_desc_t main_menu;
extern void (*actionBack)(void);
extern void watchSettingsDisplay(enum MENU_ACTIONS);

#endif