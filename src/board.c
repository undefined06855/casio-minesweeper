#include "board.h"
#include "utils.h"
#include <fxcg/heap.h>
#include <fxcg/keyboard.h>
#include <fxcg/display.h>
#include <fxcg/rtc.h>
#include <fxcg/misc.h>

void _incrementMineCount(Board* board, int row, int col, void* mineCount) {
    if (*Board_getCell(board, row, col) == (kTileTypeMine | COVER_TILE_BIT)) {
        (*(char*)mineCount)++;
    }
}

void Board_create(Board* board, int width, int height, int mines) {
    board->width = width;
    board->height = height;
    board->mines = mines;

    board->row = board->width / 2;
    board->col = board->height / 2;

    board->offsetX = (LCD_WIDTH_PX / 2) - (width * 24 / 2);
    board->offsetY = ((LCD_HEIGHT_PX - 24) / 2) - (height * 24 / 2);

    board->shakeX = 0;
    board->shakeY = 0;

    board->won = false;
    board->lost = false;

    board->firstReveal = true;

    board->startTicks = RTC_GetTicks();
    board->endTicks = -1;
    board->endAnimationFinished = false;

    // calloc reimplementation (sys_calloc crashes???)
    board->data = sys_malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        board->data[i] = 0;
    }

    // place mines
    for (int i = 0; i < mines; i++) {
        int row = Utils_randrange(0, height);
        int col = Utils_randrange(0, width);

        char* cell = Board_getCell(board, row, col);

        if (*cell == (kTileTypeMine | COVER_TILE_BIT)) {
            i--;
            continue;
        }

        *cell = kTileTypeMine | COVER_TILE_BIT;
    }

    // and place other tiles around the mines
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            char* cell = Board_getCell(board, row, col);

            if (*cell == (kTileTypeMine | COVER_TILE_BIT)) continue;

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
            int x = 24*col + board->offsetX + board->shakeX;
            int y = 24*row + board->offsetY + board->shakeY;

            if (x < -24 || y < -24 || x > LCD_WIDTH_PX || y > LCD_HEIGHT_PX) continue;

            Utils_drawSpriteAtPos(*Board_getCell(board, row, col), x, y);
        }
    }

    // draw cursor (maybe make this a sprite or look better in general?)
    int x = 24 * board->col + board->offsetX + board->shakeX;
    int y = 24 * board->row + board->offsetY + board->shakeY;
    Bdisp_Rectangle(
        x, y,
        x + 24, y + 24,
        COLOR_BLACK
    );

    Bdisp_Rectangle(
        x - 1, y - 1,
        x + 25, y + 25,
        COLOR_BLACK
    );

    Board_drawEndAnimation(board);
}

void _clearAndFillBuffer(unsigned char* buffer, int number) {
    for (int i = 0; i < 12; i++) buffer[i] = 0;
    itoa(number, buffer);
}

void Board_drawStatusArea(Board* board) {
    int x = 24;
    int y = 2;

    // draw status bar help text for the first 5 seconds
    // else draw information
    if (!RTC_Elapsed_ms(board->startTicks, 5000)) {
        PrintMini(&x, &y, "F1", 1 << 6, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
        PrintMini(&x, &y, " - Flag | ", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
        PrintMini(&x, &y, "F6", 1 << 6, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
        PrintMini(&x, &y, " - Reveal", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
        return;
    }

    if (board->endAnimationFinished) {
        PrintMini(&x, &y, "(Press any button to return)", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
        return;
    }

    // "8x8 (9 mines) 4/9 flagged"
    // is there a way to make this neater? not really, right?

    unsigned char buf[12]; // not sure if itll ever use 12 bytes but that's what's in the example!

    _clearAndFillBuffer(buf, board->width);
    PrintMini(&x, &y, (const char*)buf, 1 << 6, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, "x", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    _clearAndFillBuffer(buf, board->height);
    PrintMini(&x, &y, (const char*)buf, 1 << 6, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, " (", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    _clearAndFillBuffer(buf, board->mines);
    PrintMini(&x, &y, (const char*)buf, 1 << 6, 0xffffffff, 0, 0, COLOR_TEAL, COLOR_WHITE, true, 0);
    const char* str = " mines)";
    if (board->mines == 1) str = " mine)";
    PrintMini(&x, &y, str, 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);

    int flagCount = 0;
    for (int i = 0; i < board->width * board->height; i++) {
        if (board->data[i] & FLAG_TILE_BIT) flagCount++;
    }
    _clearAndFillBuffer(buf, flagCount);
    PrintMini(&x, &y, (const char*)buf, 1 << 6, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, "/", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    _clearAndFillBuffer(buf, board->mines);
    PrintMini(&x, &y, (const char*)buf, 1 << 6, 0xffffffff, 0, 0, COLOR_TEAL, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, " flagged", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
}

#define RAINBOW_LENGTH 12
#define ANIMATION_LENGTH RAINBOW_LENGTH * 4
color_t rainbow[RAINBOW_LENGTH] = {
    0xF800, 0xFBE0, 0xFFE0, 0x7FE0, 0x07E0, 0x07EF, 0x07FF, 0x03FF, 0x001F, 0x781F, 0xF81F, 0xF80F
};

void Board_drawEndAnimation(Board* board) {
    if (!board->won && !board->lost) return;

    const char* str;
    int speed;

    if (board->won)  str = "GAME COMPLETE!";
    else             str = "GAME OVER!";

    if (board->won) speed = 100;
    else            speed = 10;

    for (int i = 0; i <= ANIMATION_LENGTH; i++) {
        color_t col;
        if (i == ANIMATION_LENGTH) col = COLOR_BLACK;
        else col = rainbow[i % RAINBOW_LENGTH];
        if (RTC_Elapsed_ms(board->endTicks, i*speed)) {
            PrintCXY(i*2 + 24, i*2 + 24, str, TEXT_MODE_TRANSPARENT_BACKGROUND, -1, col, COLOR_WHITE, true, 0);
        }
    }

    if (RTC_Elapsed_ms(board->endTicks, ANIMATION_LENGTH * speed)) {
        board->endAnimationFinished = true;
    }
}

#undef RAINBOW_LENGTH

// returns true if the game should be exited back to the setup screen
bool Board_handleKeypress(Board* board, int key) {
    if (board->endAnimationFinished) {
        if (key) return true;
    }

    if (board->won || board->lost) return false;

    switch (key) {
        case KEY_PRGM_UP: {
            board->row--;
            if (board->row < 0) board->row = board->height - 1;
            Board_updateOffset(board);
        } break;

        case KEY_PRGM_DOWN: {
            board->row++;
            if (board->row > board->height - 1) board->row = 0;
            Board_updateOffset(board);
        } break;

        case KEY_PRGM_LEFT: {
            board->col--;
            if (board->col < 0) board->col = board->width - 1;
            Board_updateOffset(board);
        } break;

        case KEY_PRGM_RIGHT: {
            board->col++;
            if (board->col > board->width - 1) board->col = 0;
            Board_updateOffset(board);
        } break;

        case KEY_PRGM_F1: {
            Board_flag(board, board->row, board->col);
        } break;

        case KEY_PRGM_F6: {
            // false for no force
            Board_revealSingleCell(board, board->row, board->col, false);
        } break;

        case KEY_PRGM_EXIT: {
            return true;
        }
    }

    return false;
}

void Board_updateOffset(Board* board) {
    const int pad = 80;
    int left = 24 * board->col + board->offsetX + board->shakeX;
    int top = 24 * board->row + board->offsetY + board->shakeY;
    int right = left + 24;
    int bottom = top + 24;

    if (board->width * 24 + 24 > LCD_WIDTH_PX) {
        while (left < pad) {
            board->offsetX--;
            left = 24 * board->col + board->offsetX + board->shakeX;
        }

        while (right > LCD_WIDTH_PX - pad) {
            board->offsetX++;
            right = 24 * board->col + board->offsetX + board->shakeX + 24;
        }
    }

    if (board->height * 24 + 24 > LCD_HEIGHT_PX) {
        while (top < pad) {
            board->offsetY++;
            top = 24 * board->row + board->offsetY + board->shakeY;
        }

        while (bottom > LCD_HEIGHT_PX - pad) {
            board->offsetY--;
            bottom = 24 * board->row + board->offsetY + board->shakeY + 24;
        }
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
        while (*cell == (kTileTypeMine | COVER_TILE_BIT) || *cell != (kTileTypeZero | COVER_TILE_BIT)) {
            Board* newBoard = sys_malloc(sizeof(Board));
            Board_create(newBoard, board->width, board->height, board->mines);

            sys_free(board->data);
            board->data = newBoard->data;
            sys_free(newBoard); // does not free newboard->data! that's what Board_free does!

            cell = Board_getCell(board, row, col);
        }
    }

    // if it's not covered, count surrounding mines
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

    if (*cell == kTileTypeMine) {
        // uh oh!
        Board_kablooey(board);
        return;
    }

    Board_checkWinCondition(board);

    if (*cell == kTileTypeZero) {
        // reveal surrounding cells
        Board_revealSurroundingCells(board, row, col);
        return;
    }
}

void Board_kablooey(Board* board) {
    board->lost = true;
    board->endTicks = RTC_GetTicks();

    // set certain tiles to special ones
    for (int i = 0; i < board->width * board->height; i++) {
        char* cell = &board->data[i];

        // mark all incorrect flags
        if (Board_cellIsFlagged(cell) && *cell != (kTileTypeMine | COVER_TILE_BIT | FLAG_TILE_BIT)) {
            *cell = kTileTypeIncorrectFlag;
            continue;
        }

        // and uncover all mines
        if (*cell == (kTileTypeMine | COVER_TILE_BIT)) {
            *cell &= ~COVER_TILE_BIT;
            continue;
        }
    }

    (*Board_getCell(board, board->row, board->col)) = kTileTypeHitMine;
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
        if (
            Board_cellIsCovered(&board->data[i])
        &&  board->data[i] != (kTileTypeMine | COVER_TILE_BIT)
        &&  board->data[i] != (kTileTypeMine | COVER_TILE_BIT | FLAG_TILE_BIT)
        ) return;
    }

    // else you win
    board->won = true;
    board->endTicks = RTC_GetTicks();
}

char* Board_getCell(Board* board, int row, int col) {
    return &board->data[row*board->width + col];
}
