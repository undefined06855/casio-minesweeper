#include "menu.h"
#include "utils.h"
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>
#include <fxcg/misc.h>
#include <fxcg/rtc.h>

void Menu_create(Menu* menu) {
    menu->width = 9;
    menu->height = 9;
    menu->mines = 10;

    menu->settingCursorPosition = 0;
    menu->settingRow = 0;

    menu->notification = 0x0;
    menu->notificationTime = -1;

    menu->board = 0x0;
}

void Menu_free(Menu* menu) {
    if (menu->board) { Board_free(menu->board); }
    sys_free(menu);
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

    int charWidth = 0;
    PrintMini(&charWidth, &y, "0", TEXT_MODE_NORMAL, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, false, 0);

    PrintMini(&x, &y, "width: ", TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    cursorStartPositions[0] = x;
    Utils_clearAndFillBuffer(buf, menu->width);
    PrintMini(&x, &y, (const char*)buf, TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    x = 24;
    y += 24;

    PrintMini(&x, &y, "height: ", TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    cursorStartPositions[1] = x;
    Utils_clearAndFillBuffer(buf, menu->height);
    PrintMini(&x, &y, (const char*)buf, TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    x = 24;
    y += 24;

    PrintMini(&x, &y, "mines: ", TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    cursorStartPositions[2] = x;
    Utils_clearAndFillBuffer(buf, menu->mines);
    PrintMini(&x, &y, (const char*)buf, TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    if (menu->width * menu->height != 0) {
        PrintMini(&x, &y, " (", TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
        Utils_clearAndFillBuffer(buf, (100*menu->mines) / (menu->width * menu->height));
        PrintMini(&x, &y, (const char*)buf, TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
        PrintMini(&x, &y, "%)", TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
        x = 24;
        y += 24;
    } else {
        // if either are zero we'll get a divide by zero error, so hardcode 0%
        // TODO: colour this
        PrintMini(&x, &y, " (0%)", TEXT_MODE_TRANSPARENT_BACKGROUND, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    }

    PrintCXY(LCD_WIDTH_PX / 2 - 72, LCD_HEIGHT_PX - 80, "Continue", TEXT_MODE_TRANSPARENT_BACKGROUND, -1, COLOR_BLACK, COLOR_WHITE, true, 0);

    if (menu->settingRow != 3) {
        // calculator cursor position
        int x = cursorStartPositions[menu->settingRow] + Menu_getCurrentSettingValueLength(menu) * charWidth - menu->settingCursorPosition * charWidth;
        int y = menu->settingRow * 24 + 50;

        Bdisp_Rectangle(
            x, y,
            x + 1, y + 18,
            TEXT_COLOR_BLACK
        );
    } else {
        int x = LCD_WIDTH_PX / 2 - 72;
        int y = LCD_HEIGHT_PX - 80;

        Bdisp_Rectangle(
            x - 4, y - 4,
            x + 8*18 + 4, y + 18 + 4,
            TEXT_COLOR_BLACK
        );
    }

    int rectX = LCD_WIDTH_PX * 0.8 - 20;
    int rectY = LCD_HEIGHT_PX * 0.5 - 30;

    int textCol = TEXT_COLOR_BLACK;

    if (menu->width != 0 && menu->height != 0) {
        // if either are zero we'll get divide by zero error again so don't draw
        // a rectangle if so

        int rectWidth, rectHeight;

        const int maxRectSize = LCD_HEIGHT_PX * 0.5;

        if (menu->width > menu->height) {
            rectWidth = maxRectSize;
            rectHeight = (maxRectSize * (1000*menu->height / menu->width)) / 1000;
        } else {
            rectWidth = (maxRectSize * (1000*menu->width / menu->height)) / 1000;
            rectHeight = maxRectSize;
        }

        Bdisp_Rectangle(
            rectX - rectWidth / 2, rectY - rectHeight / 2,
            rectX + rectWidth / 2, rectY + rectHeight / 2,
            TEXT_COLOR_PURPLE
        );

        // TODO: show mines percentage
    } else {
        textCol = TEXT_COLOR_RED;
    }

    // draw the text inside by first measuring it and then drawing
    // oh hey we can use the buffer from earlier
    // PrintMiniMini mode should be zero (we dont need any special handling)
    int width = 0;
    int _ = 0;

    Utils_clearAndFillBuffer(buf, menu->width);
    PrintMiniMini(&width, &_, (const char*)buf, 0, textCol, true);
    PrintMiniMini(&width, &_, "x", 0, textCol, true);
    Utils_clearAndFillBuffer(buf, menu->height);
    PrintMiniMini(&width, &_, (const char*)buf, 0, textCol, true);

    int textX = rectX - width / 2;
    int textY = rectY - 5;

    Utils_clearAndFillBuffer(buf, menu->width);
    PrintMiniMini(&textX, &textY, (const char*)buf, 0, textCol, false);
    PrintMiniMini(&textX, &textY, "x", 0, textCol, false);
    Utils_clearAndFillBuffer(buf, menu->height);
    PrintMiniMini(&textX, &textY, (const char*)buf, 0, textCol, false);
}

void Menu_drawStatusArea(Menu* menu) {
    if (menu->board) {
        Board_drawStatusArea(menu->board);
    }

    if (menu->notification) {
        int x = 24;
        int y = 2;
        PrintMini(&x, &y, menu->notification, 1 << 6, 0xffffffff, 0, 0, COLOR_RED, COLOR_WHITE, true, 0);

        if (RTC_Elapsed_ms(menu->notificationTime, 1000)) {
            menu->notification = 0x0;
            menu->notificationTime = -1;
        }
    }
}

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
                menu->settingRow = 3;
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
                Menu_fixMineCount(menu);
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
            if (Menu_getCurrentSettingValueLength(menu) == Menu_getCurrentSettingValueLimit(menu)) break;

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
    if (menu->width < 2) {
        menu->notification = "Width cannot be <2!";
        menu->notificationTime = RTC_GetTicks();
        return;
    }

    if (menu->height < 2) {
        menu->notification = "Height cannot be <2!";
        menu->notificationTime = RTC_GetTicks();
        return;
    }

    if (menu->mines == 0) {
        menu->notification = "Mines must be >1!";
        menu->notificationTime = RTC_GetTicks();
        return;
    }

    menu->board = sys_malloc(sizeof(Board));
    Board_create(menu->board, menu->width, menu->height, menu->mines, /* real board */ false);
}

int* Menu_getCurrentSetting(Menu* menu) {
    switch (menu->settingRow) {
        case 0: return &menu->width;
        case 1: return &menu->height;
        case 2: return &menu->mines;
    }

    return 0x0;
}

int Menu_getCurrentSettingValueLimit(Menu* menu) {
    if (menu->settingRow == 2) return 3;
    else return 2;
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
