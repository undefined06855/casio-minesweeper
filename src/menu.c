#include "menu.h"
#include "utils.h"
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>
#include <fxcg/misc.h>

void Menu_create(Menu* menu) {
    menu->width = 9;
    menu->height = 9;
    menu->mines = 10;

    menu->settingCursorPosition = 0;
    menu->settingRow = 0;

    menu->board = 0x0;
}

void Menu_free(Menu* menu) {
    if (menu->board) { Board_free(menu->board); }
    sys_free(menu);
}

void _clearAndFillBuffer2(unsigned char* buffer, int number) {
    for (int i = 0; i < 12; i++) buffer[i] = 0;
    itoa(number, buffer);
}

void Menu_draw(Menu* menu) {
    if (menu->board) {
        Board_draw(menu->board);
        return;
    }

    // TODO: draw a background that shows the board size + mines

    PrintCXY(10, 10, "Setup", TEXT_MODE_TRANSPARENT_BACKGROUND, -1, COLOR_BLACK, COLOR_WHITE, true, 0);

    // PrintMini(&x, &y, "F1", 1 << 6, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    // PrintMini(&x, &y, " - Continue", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);

    unsigned char buf[12];
    int x = 24;
    int y = 50;
    int cursorStartPositions[3];

    int width = 0;
    PrintMini(&width, &y, "0", TEXT_MODE_NORMAL, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, false, 0);

    PrintMini(&x, &y, "width: ", TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    cursorStartPositions[0] = x;
    _clearAndFillBuffer2(buf, menu->width);
    PrintMini(&x, &y, (const char*)buf, TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    x = 24;
    y += 24;

    PrintMini(&x, &y, "height: ", TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    cursorStartPositions[1] = x;
    _clearAndFillBuffer2(buf, menu->height);
    PrintMini(&x, &y, (const char*)buf, TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    x = 24;
    y += 24;

    PrintMini(&x, &y, "mines: ", TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    cursorStartPositions[2] = x;
    _clearAndFillBuffer2(buf, menu->mines);
    PrintMini(&x, &y, (const char*)buf, TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    x = 24;
    y += 24;

    PrintCXY(LCD_WIDTH_PX / 2 - 72, LCD_HEIGHT_PX - 54, "Continue", TEXT_MODE_TRANSPARENT_BACKGROUND, -1, COLOR_BLACK, COLOR_WHITE, true, 0);

    if (menu->settingRow != 3) {
        // calculator cursor position
        int x = cursorStartPositions[menu->settingRow] + Menu_getCurrentSettingValueLength(menu) * width - menu->settingCursorPosition * width;
        int y = menu->settingRow * 24 + 50;

        Bdisp_Rectangle(
            x, y,
            x + 1, y + 18,
            COLOR_BLACK
        );
    } else {
        int x = LCD_WIDTH_PX / 2 - 72;
        int y = LCD_HEIGHT_PX - 54;

        Bdisp_Rectangle(
            x - 4, y - 4,
            x + 8*18 + 4, y + 18 + 4,
            COLOR_BLACK
        );
    }
}

void Menu_drawStatusArea(Menu* menu) {
    if (menu->board) {
        Board_drawStatusArea(menu->board);
    }
}

// returns true if the program should exit
void Menu_handleKeypress(Menu* menu, int key) {
    if (menu->board) {
        bool shouldExit = Board_handleKeypress(menu->board, key);
        if (shouldExit) {
            Board_free(menu->board);
            menu->board = 0x0;
            menu->settingRow = 0;
            menu->settingCursorPosition = 0;
        }

        return;
    }

    switch (key) {
        case KEY_PRGM_F1: {
            Menu_begin(menu);
        } break;

        case KEY_PRGM_RETURN: { // misnomer? this is exe key
            menu->settingRow++;
            if (menu->settingRow > 3) {
                Menu_begin(menu);
            }
        } break;

        case KEY_PRGM_UP: {
            menu->settingRow--;
            menu->settingCursorPosition = 0;
            if (menu->settingRow < 0) menu->settingRow++;
        } break;

        case KEY_PRGM_DOWN: {
            menu->settingRow++;
            menu->settingCursorPosition = 0;
            if (menu->settingRow > 3) menu->settingRow--;
        } break;

        case KEY_PRGM_DEL:
        case KEY_PRGM_0:
        case KEY_PRGM_1:
        case KEY_PRGM_2:
        case KEY_PRGM_3:
        case KEY_PRGM_4:
        case KEY_PRGM_5:
        case KEY_PRGM_6:
        case KEY_PRGM_7:
        case KEY_PRGM_8:
        case KEY_PRGM_9:
        case KEY_PRGM_LEFT:
        case KEY_PRGM_RIGHT: {
            Menu_handleTextKeypress(menu, key);
        } break;
    }
}

void Menu_handleTextKeypress(Menu* menu, int key) {
    if (!Menu_getCurrentSetting(menu)) return;

    // settingCursorPosition is zero if the cursor is at the very right!!!

    switch (key) {
        case KEY_PRGM_LEFT: {
            int max = Menu_getCurrentSettingValueLength(menu);
            menu->settingCursorPosition++;
            if (menu->settingCursorPosition > max) menu->settingCursorPosition--;
        } break;

        case KEY_PRGM_RIGHT: {
            menu->settingCursorPosition--;
            if (menu->settingCursorPosition < 0) menu->settingCursorPosition++;
        } break;

        case KEY_PRGM_DEL: {
            int* value = Menu_getCurrentSetting(menu);
            if (Menu_getCurrentSettingValueLength(menu) == 1) {
                *value = 0;
                menu->settingCursorPosition = 0;
                break;
            }

            int power = 1;
            for (int i = 0; i < menu->settingCursorPosition; i++) power *= 10;

            int left = *value / power;
            int right = *value % power;

            left /= 10; // remove the last digit of the left side

            *value = left * power + right;

            Menu_fixMineCount(menu);
        } break;

        default: {
            // no numbers greater than 2 digits
            if (Menu_getCurrentSettingValueLength(menu) == 2) break;

            int* value = Menu_getCurrentSetting(menu);
            int keyValue = Utils_keyToNumber(key);
            int power = 1;
            for (int i = 0; i < menu->settingCursorPosition; i++) power *= 10;

            int left = *value / power;
            int right = *value % power;

            *value = left * power * 10 + keyValue * power + right;

            Menu_fixMineCount(menu);
        } break;
    }
}

void Menu_fixMineCount(Menu* menu) {
    if (menu->mines > (menu->width * menu->height) * 0.9) {
        menu->mines = (menu->width * menu->height) * 0.9;
    }
}

void Menu_begin(Menu* menu) {
    menu->board = sys_malloc(sizeof(Board));
    Board_create(menu->board, menu->width, menu->height, menu->mines);
}

int* Menu_getCurrentSetting(Menu* menu) {
    switch (menu->settingRow) {
        case 0: return &menu->width;
        case 1: return &menu->height;
        case 2: return &menu->mines;
    }

    return 0x0;
}

// is there a nicer way to do this? this is definitely the most efficient
int Menu_getCurrentSettingValueLength(Menu* menu) {
    int value = *Menu_getCurrentSetting(menu);
    if (value < 10) return 1;
    if (value < 100) return 2;
    if (value < 1000) return 3;
    if (value < 10000) return 4;
    if (value < 100000) return 5;
    return 6;
}
