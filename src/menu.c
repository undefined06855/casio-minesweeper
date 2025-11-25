#include "menu.h"
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>

void Menu_create(Menu* menu) {
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

    int x = 24;
    int y = 2;

    PrintMini(&x, &y, "Setup | F1 - Continue", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
}

void Menu_handleKeypress(Menu* menu, int key) {
    if (menu->board) {
        Board_handleKeypress(menu->board, key);
        return;
    }

    switch (key) {
        case KEY_CTRL_F1: {
            menu->board = sys_malloc(sizeof(Board));
            Board_create(menu->board, LCD_WIDTH_PX / 24, (LCD_HEIGHT_PX - 24) / 24, 12);
        } break;
    }
}
