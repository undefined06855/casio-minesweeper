#pragma once
#include "board.h"

typedef struct {
    int width;
    int height;
    int mines;

    int settingCursorPosition;
    int settingRow;

    const char* notification;
    int notificationTime;

    Board* board;
} Menu;

void Menu_create(Menu* menu);
void Menu_free(Menu* menu);

void Menu_draw(Menu* menu);
void Menu_drawStatusArea(Menu* menu);
void Menu_handleKeypress(Menu* menu, int key);
void Menu_handleTextKeypress(Menu* menu, int key);

void Menu_fixMineCount(Menu* menu);

int* Menu_getCurrentSetting(Menu* menu);
int Menu_getCurrentSettingValueLength(Menu* menu);
int Menu_getCurrentSettingValueLimit(Menu* menu);

void Menu_begin(Menu* menu);
