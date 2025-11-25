#pragma once
#include "board.h"

typedef struct {
    Board* board;
} Menu;

void Menu_create(Menu* menu);
void Menu_free(Menu* menu);

void Menu_draw(Menu* menu);
void Menu_handleKeypress(Menu* board, int key);
