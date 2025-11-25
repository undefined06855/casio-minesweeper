#pragma once
#include "board.h"

typedef struct {
    int width;
    int height;
    int mines;

    Board* board;
} Menu;

void Menu_create(Menu* menu);
void Menu_free(Menu* menu);

void Menu_draw(Menu* menu);
bool Menu_handleKeypress(Menu* board, int key);
