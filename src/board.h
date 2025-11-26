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

    int shakeX;
    int shakeY;

    bool lost;
    bool won;

    bool firstReveal;

    int startTicks;
    int endTicks;
    bool endAnimationFinished;
} Board;

void Board_create(Board* board, int width, int height, int mines);
void Board_free(Board* board);

void Board_draw(Board* board);
void Board_drawStatusArea(Board* board);
void Board_drawEndAnimation(Board* board);
bool Board_handleKeypress(Board* board, int key);

void Board_updateOffset(Board* board);

void Board_flag(Board* board, int row, int col);
void Board_revealSurroundingCells(Board* board, int row, int col);
void Board_revealSingleCell(Board* board, int row, int col, bool force);
void Board_checkWinCondition(Board* board);
void Board_kablooey(Board* board);

void Board_runForSurroundingCells(Board* board, int row, int col, void(*callback)(Board*, int, int, void*), void* data);

// maybe these should just take in the char so it's one byte instead of four
// but it's easier to deref inside the function than at every single call site
// (and does 3 bytes really matter when the registers are 32 bits anyway...)
bool Board_cellIsFlagged(char* cell);
bool Board_cellIsCovered(char* cell);

char* Board_getCell(Board* board, int row, int col);
