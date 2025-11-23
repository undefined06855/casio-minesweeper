#pragma once
#include "utils.h"

typedef struct {
    char* data;
    int width;
    int height;
    int mines;

    int row;
    int col;

    int offsetX;
    int offsetY;

    bool died;
    bool won;
} Board;

Board* Board_create(int width, int height, int mines);
void Board_draw(Board* board);
void Board_handleKeypress(Board* board, int key);

void Board_flag(Board* board);
void Board_revealSurroundingTiles(Board* board, int row, int col);
void Board_revealSingleTile(Board* board, int row, int col);
void Board_checkWinCondition(Board* board);

char* Board_currentCell(Board* board);
