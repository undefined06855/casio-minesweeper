#pragma once
#include "board.h"
#include "scores.h"

typedef enum {
    PresetTypeEasy = 0,
    PresetTypeMedium = 1,
    PresetTypeHard = 2,

    PresetTypeMax, _MakePresetTypeSigned = -1
} PresetType;

typedef enum {
    SettingRowPreset = 0,
    SettingRowWidth = 1,
    SettingRowHeight = 2,
    SettingRowMines = 3,
    SettingRowContinue = 4,

    SettingRowMax, _MakeSettingRowMaxSigned = -1
} SettingRow;

typedef struct {
    PresetType preset;
    int width;
    int height;
    int mines;

    int settingCursorPosition;
    SettingRow settingRow;

    const char* notification;
    int notificationTime;

    Board* board;
    Scores* scores;
} Menu;

void Menu_create(Menu* menu);
void Menu_free(Menu* menu);

void Menu_draw(Menu* menu);
void Menu_drawStatusArea(Menu* menu);
void Menu_handleKeypress(Menu* menu, int key);
void Menu_handleTextKeypress(Menu* menu, int key);

void Menu_applyPreset(Menu* menu);
void Menu_fixMineCount(Menu* menu);

int* Menu_getCurrentSetting(Menu* menu);
bool Menu_getCurrentSettingSpecial(Menu* menu);
int Menu_getCurrentSettingValueLength(Menu* menu);
int Menu_getCurrentSettingValueLimit(Menu* menu);
const char* Menu_getPresetName(Menu* menu);
int Menu_getPresetColor(Menu* menu);

void Menu_begin(Menu* menu);
