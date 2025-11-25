#include "board.h"
#include "utils.h"
#include <fxcg/heap.h>
#include <fxcg/keyboard.h>
#include <fxcg/display.h>
#include <fxcg/rtc.h>
#include <fxcg/misc.h>

void _incrementMineCount(Board* board, int row, int col, void* mineCount) {
    if (*Board_getCell(board, row, col) == kTileTypeCoveredBomb) {
        (*(char*)mineCount)++;
    }
}

void Board_create(Board* board, int width, int height, int mines) {
    board->width = width;
    board->height = height;
    board->mines = mines;

    board->row = 0;
    board->col = 0;

    board->offsetX = 0;
    board->offsetY = 0;

    board->firstReveal = true;

    board->startTicks = RTC_GetTicks();

    // calloc reimplementation (sys_calloc crashes???)
    board->data = sys_malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        board->data[i] = 0;
    }

    // place mines
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

    // and place other tiles around the mines
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            char* cell = Board_getCell(board, row, col);

            if (*cell == kTileTypeCoveredBomb) continue;

            char mineCount = 0;
            Board_runForSurroundingCells(board, row, col, _incrementMineCount, &mineCount);

            *cell = mineCount | COVER_TILE_BIT;
        }
    }
}

void Board_free(Board* board) {
    sys_free(board->data);
    sys_free(board);
}

void Board_draw(Board* board) {
    // draw board
    for (int row = 0; row < board->height; row++) {
        for (int col = 0; col < board->width; col++) {
            drawSpriteAtPos(*Board_getCell(board, row, col), 24*col + board->offsetX, 24*row + board->offsetY);
        }
    }

    // draw cursor (maybe make this a sprite or look better in general?)
    int x = 24 * board->col;
    int y = 24 * board->row;
    Bdisp_Rectangle(
        board->offsetX + x,
        board->offsetY + y,
        board->offsetX + x + 24,
        board->offsetY + y + 24,
        COLOR_BLACK
    );

    Bdisp_Rectangle(
        board->offsetX + x - 1,
        board->offsetY + y - 1,
        board->offsetX + x + 24 + 1,
        board->offsetY + y + 24 + 1,
        COLOR_BLACK
    );

    Board_drawStatusBar(board);
}

void _clearAndFillBuffer(unsigned char* buffer, int number) {
    for (int i = 0; i < 11; i++) buffer[i] = 0;
    itoa(number, buffer);
}

void Board_drawStatusBar(Board* board) {
    // draw status bar help text for the first 3s
    // else draw information
    int x = 24;
    int y = 2;

    if (board->won) {
        PrintMini(&x, &y, "You win! Press any key to continue.", 1 << 6, 0xffffffff, 0, 0, COLOR_CHARTREUSE, COLOR_WHITE, true, 0);
        return;
    }

    if (board->lost) {
        PrintMini(&x, &y, "You lose! Press any key to continue.", 1 << 6, 0xffffffff, 0, 0, COLOR_FIREBRICK, COLOR_WHITE, true, 0);
        return;
    }

    if (!RTC_Elapsed_ms(board->startTicks, 3000)) {
        PrintMini(&x, &y, "F1 - Flag | F2 - Reveal", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
        return;
    }

    // "8x8 (9 mines) 4/9 flagged"
    // is there a way to make this neater? not really, right?

    unsigned char* buf = sys_malloc(11);

    _clearAndFillBuffer(buf, board->width);
    PrintMini(&x, &y, (const char*)buf, 1 << 6, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, "x", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    _clearAndFillBuffer(buf, board->height);
    PrintMini(&x, &y, (const char*)buf, 1 << 6, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, " (", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    _clearAndFillBuffer(buf, board->mines);
    PrintMini(&x, &y, (const char*)buf, 1 << 6, 0xffffffff, 0, 0, COLOR_TEAL, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, " mines) ", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);

    int flagCount = 0;
    for (int i = 0; i < board->width * board->height; i++) {
        if (board->data[i] & FLAG_TILE_BIT) flagCount++;
    }
    _clearAndFillBuffer(buf, flagCount);
    PrintMini(&x, &y, (const char*)buf, 1 << 6, 0xffffffff, 0, 0, COLOR_STEELBLUE, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, "/", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    _clearAndFillBuffer(buf, board->mines);
    PrintMini(&x, &y, (const char*)buf, 1 << 6, 0xffffffff, 0, 0, COLOR_TEAL, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, " flagged", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
}

void Board_handleKeypress(Board* board, int key) {
    if (board->won || board->lost) return;

    switch (key) {
        case KEY_CTRL_UP: {
            board->row--;
            if (board->row < 0) board->row = board->height - 1;
        } break;

        case KEY_CTRL_DOWN: {
            board->row++;
            if (board->row > board->height - 1) board->row = 0;
        } break;

        case KEY_CTRL_LEFT: {
            board->col--;
            if (board->col < 0) board->col = board->width - 1;
        } break;

        case KEY_CTRL_RIGHT: {
            board->col++;
            if (board->col > board->width - 1) board->col = 0;
        } break;

        case KEY_CTRL_F1: {
            Board_flag(board, board->row, board->col);
        } break;

        case KEY_CTRL_F2: {
            // false for no force
            Board_revealSingleCell(board, board->row, board->col, false);
        } break;
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

void _revealCellIfNotFlagged(Board* board, int row, int col, void* _unused) {
    char* cell = Board_getCell(board, row, col);
    if (!Board_cellIsFlagged(cell) && Board_cellIsCovered(cell)) {
        Board_revealSingleCell(board, row, col, true);
    }
}

void _incrementFlagCount(Board* board, int row, int col, void* flagCount) {
    if (*Board_getCell(board, row, col) & FLAG_TILE_BIT) {
        (*(char*)flagCount)++;
    }
}

void Board_revealSingleCell(Board* board, int row, int col, bool force) {
    char* cell = Board_getCell(board, row, col);

    if (board->firstReveal) {
        // oooh exciting exciting
        board->firstReveal = false;

        // regenerate everything while first click is a mine or not a zero tile
        // (we want to be generous here, don't just give the player one tile)
        // this is pretty scary i hope it doesnt leak memory
        while (*cell == kTileTypeCoveredBomb || *cell != (kTileTypeZero | COVER_TILE_BIT)) {
            Board* newBoard = sys_malloc(sizeof(Board));
            Board_create(newBoard, board->width, board->height, board->mines);

            sys_free(board->data);
            board->data = newBoard->data;
            sys_free(newBoard); // does not free newboard->data! that's what Board_free does!

            cell = Board_getCell(board, row, col);
        }
    }

    // if it's not covered, count surrounding bombs
    // then if it's equal to the value on the tile, auto reveal non-flagged cells
    if (!Board_cellIsCovered(cell) && *cell != kTileTypeZero) {
        char flagCount = 0;
        Board_runForSurroundingCells(board, row, col, _incrementFlagCount, &flagCount);

        if (*cell == flagCount) {
            Board_runForSurroundingCells(board, row, col, _revealCellIfNotFlagged, 0x0);
        }

        return;
    }

    // if it's flagged, unflag
    if (Board_cellIsFlagged(cell)) {
        *cell ^= FLAG_TILE_BIT;

        // if we NEED to reveal this cell, continue going, else just stop here
        if (!force) return;
    }

    *cell &= ~COVER_TILE_BIT; // unset covered bit
    *cell &= ~FLAG_TILE_BIT;

    if (*cell == kTileTypeBomb) {
        // uh oh!
        board->lost = true;
        return;
    }

    Board_checkWinCondition(board);

    if (*cell == kTileTypeZero) {
        // reveal surrounding cells
        Board_revealSurroundingCells(board, row, col);
        return;
    }
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
    // if any cells are covered still and aren't mines, return
    for (int i = 0; i < board->width * board->height; i++) {
        if (Board_cellIsCovered(&board->data[i]) && board->data[i] != kTileTypeCoveredBomb) return;
    }

    // else you win
    board->won = true;
}

char* Board_getCell(Board* board, int row, int col) {
    return &board->data[row*board->width + col];
}
