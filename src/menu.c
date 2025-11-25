#include "menu.h"
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>

void Menu_create(Menu* menu) {
    menu->width = LCD_WIDTH_PX / 24;
    menu->height =  (LCD_HEIGHT_PX - 24) / 24;
    menu->mines = 14;

    menu->board = 0x0;
}

void Menu_free(Menu* menu) {
    if (menu->board) { Board_free(menu->board); }
    sys_free(menu);
}

#include <fxcg/misc.h>
#include "keyboard.h"
void Menu_draw(Menu* menu) {
    if (menu->board) {
        Board_draw(menu->board);
        return;
    }

    int x = 24;
    int y = 2;

    PrintXY(1, 1, "  Setup", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);

    PrintMini(&x, &y, "F1", 1 << 6, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, " - Continue", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);

    // debug
    // unsigned char buf[12];
    // itoa(Key_pressed(), buf);
    // PrintMini(&x, &y, (const char*)buf, 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
}

// returns true if the program should exit
bool Menu_handleKeypress(Menu* menu, int key) {
    if (menu->board) {
        bool shouldExit = Board_handleKeypress(menu->board, key);
        if (shouldExit) {
            Board_free(menu->board);
            menu->board = 0x0;
        }

        return false;
    }

    switch (key) {
        case KEY_PRGM_EXIT:
        case KEY_PRGM_MENU:
            return true;

        case KEY_PRGM_F1: {
            menu->board = sys_malloc(sizeof(Board));
            Board_create(menu->board, menu->width, menu->height, menu->mines);
            return false;
        }
    }

    return false;
}
