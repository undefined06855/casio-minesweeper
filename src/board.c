#include "board.h"
#include "utils.h"
#include <fxcg/heap.h>
#include <fxcg/keyboard.h>
#include <fxcg/display.h>

void _incrementMineCount(Board* board, int row, int col, void* mineCount) {
    if (*Board_getCell(board, row, col) == kTileTypeCoveredBomb) {
        (*(char*)mineCount)++;
    }
}

Board* Board_create(int width, int height, int mines) {
    Board* board = sys_malloc(sizeof(Board));

    board->width = width;
    board->height = height;
    board->mines = mines;

    board->row = 0;
    board->col = 0;

    board->offsetX = 0;
    board->offsetY = 0;

    // calloc reimplementation (sys_calloc crashes???)
    board->data = sys_malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        board->data[i] = 0;
    }

    for (int i = 0; i < mines; i++) {
        int row = randrange(0, height);
        int col = randrange(0, width);

        char* cell = Board_getCell(board, row, col);

        if (*cell == kTileTypeBomb) {
            i--;
            continue;
        }

        *cell = kTileTypeBomb | COVER_TILE_BIT;
    }

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            char* tile = Board_getCell(board, row, col);

            if (*tile == kTileTypeCoveredBomb) continue;

            char mineCount = 0;
            Board_runForSurroundingCells(board, row, col, _incrementMineCount, &mineCount);

            *tile = mineCount | COVER_TILE_BIT;
        }
    }

    return board;
}

void Board_draw(Board* board) {
    for (int row = 0; row < board->height; row++) {
        for (int col = 0; col < board->width; col++) {
            drawSpriteAtPos(*Board_getCell(board, row, col), 24*col + board->offsetX, 24*row + board->offsetY);
        }
    }

    int x = 24 * board->col;
    int y = 24 * board->row;
    Bdisp_Rectangle(
        board->offsetX + x,
        board->offsetY + y,
        board->offsetX + x + 24,
        board->offsetY + y + 24,
        TEXT_COLOR_BLACK
    );

    Bdisp_Rectangle(
        board->offsetX + x - 1,
        board->offsetY + y - 1,
        board->offsetX + x + 24 + 1,
        board->offsetY + y + 24 + 1,
        TEXT_COLOR_BLACK
    );

    if (board->died) {
        Bdisp_MMPrint(0, 0, (unsigned char*)"you lose!", TEXT_MODE_NORMAL, 0xffffffff, 0, 0, TEXT_COLOR_BLACK, TEXT_COLOR_WHITE, true, 0);
    }

    if (board->won) {
        Bdisp_MMPrint(0, 0, (unsigned char*)"you win!", TEXT_MODE_NORMAL, 0xffffffff, 0, 0, TEXT_COLOR_BLACK, TEXT_COLOR_WHITE, true, 0);
    }
}

void Board_handleKeypress(Board* board, int key) {
    if (board->won || board->died) return;

    switch (key) {
        case KEY_CTRL_UP:
            board->row--;
            if (board->row < 0) board->row = board->height - 1;
            break;

        case KEY_CTRL_DOWN:
            board->row++;
            if (board->row > board->height - 1) board->row = 0;
            break;

        case KEY_CTRL_LEFT:
            board->col--;
            if (board->col < 0) board->col = board->width - 1;
            break;

        case KEY_CTRL_RIGHT:
            board->col++;
            if (board->col > board->width - 1) board->col = 0;
            break;

        case KEY_CTRL_F1:
            Board_flag(board, board->row, board->col);
            break;

        case KEY_CTRL_F2:
            // false for no force
            Board_revealSingleCell(board, board->row, board->col, false);
            break;
    }
}

void Board_flag(Board* board, int row, int col) {
    char* cell = Board_getCell(board, row, col);

    if (!Board_cellIsCovered(cell)) {
        // you shouldnt be able to flag revealed cells
        return;
    }

    *cell ^= FLAG_TILE_BIT;
}

void Board_revealSingleCell(Board* board, int row, int col, bool force) {
    char* cell = Board_getCell(board, row, col);

    // if it's flagged, unflag
    if (Board_cellIsFlagged(cell)) {
        *cell &= COVER_TILE_BIT;

        // if we NEED to reveal this tile, continue going, else just stop here
        if (!force) return;
    }

    // if it's not covered, count surrounding bombs
    // then if it's equal to the value on the cell, auto reveal non-flagged tiles
    if (!Board_cellIsCovered(cell)) {

    }

    *cell &= ~COVER_TILE_BIT; // unset covered bit
    *cell &= ~FLAG_TILE_BIT;

    if (*cell == kTileTypeZero) {
        // reveal surrounding tiles
        Board_revealSurroundingCells(board, row, col);
        Board_checkWinCondition(board);
        return;
    }

    if (*cell == kTileTypeBomb) {
        // uh oh!
        board->died = true;
        return;
    }

    Board_checkWinCondition(board);
}


// will reveal all surrounding tiles that aren't covered
// note: force is enabled here, since we want to always uncover the tiles,
// regardless of if they're flagged or not
void _revealCellIfCovered(Board* board, int row, int col, void* _unused) {
    if (Board_cellIsCovered(Board_getCell(board, row, col))) {
        Board_revealSingleCell(board, row, col, true);
    }
}

void Board_revealSurroundingCells(Board* board, int row, int col) {
    Board_runForSurroundingCells(board, row, col, _revealCellIfCovered, 0x0);
}

void Board_runForSurroundingCells(Board* board, int row, int col, void(*callback)(Board*, int, int, void*), void* data) {
    int width = board->width;
    int height = board->height;

    if (row != 0) {
        if (col != 0) { callback(board, row - 1, col - 1, data); }
        callback(board, row - 1, col, data);
        if (col != width - 1) { callback(board, row - 1, col + 1, data); }
    }

    if (col != 0) { callback(board, row, col - 1, data); }
    if (col != width - 1) { callback(board, row, col + 1, data); }

    if (row != height - 1) {
        if (col != 0) { callback(board, row + 1, col - 1, data); }
        callback(board, row + 1, col, data);
        if (col != width - 1) { callback(board, row + 1, col + 1, data); }
    }
}

bool Board_cellIsFlagged(char* cell) {
    return *cell & FLAG_TILE_BIT;
}

bool Board_cellIsCovered(char* cell) {
    return *cell & COVER_TILE_BIT;
}

void Board_checkWinCondition(Board* board) {
    // if any tiles are covered still and aren't mines, return
    for (int i = 0; i < board->width * board->height; i++) {
        if (Board_cellIsCovered(&board->data[i]) && board->data[i] != kTileTypeCoveredBomb) return;
    }

    // else you win
    board->won = true;
}

char* Board_getCell(Board* board, int row, int col) {
    return &board->data[row*board->width + col];
}
