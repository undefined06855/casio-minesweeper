#include "board.h"
#include "utils.h"
#include <fxcg/heap.h>
#include <fxcg/keyboard.h>
#include <fxcg/display.h>

Board* Board_create(int width, int height, int mines) {
    Board* board = sys_malloc(sizeof(Board));

    board->width = width;
    board->height = height;
    board->mines = mines;

    board->row = 0;
    board->col = 0;

    board->offsetX = 0;
    board->offsetY = 0;

    // CALLOC CRASHES???
    board->data = sys_malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        board->data[i] = 0;
    }

    for (int i = 0; i < mines; i++) {
        int row = randrange(0, height);
        int col = randrange(0, width);

        if (board->data[row*width + col] == kTileTypeBomb) {
            i--;
            continue;
        }

        board->data[row*width + col] = kTileTypeBomb | COVER_TILE_BIT;
    }

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            char tile = board->data[row*width + col];

            if (tile == kTileTypeCoveredBomb) continue;

            char mineCount = 0;

            if (row != 0) {
                if (col != 0 &&       board->data[(row-1)*width + col-1] == kTileTypeCoveredBomb) mineCount++;
                if (                  board->data[(row-1)*width + col] == kTileTypeCoveredBomb) mineCount++;
                if (col != width-1 && board->data[(row-1)*width + col+1] == kTileTypeCoveredBomb) mineCount++;
            }

            if (col != 0 &&       board->data[row*width + col-1] == kTileTypeCoveredBomb) mineCount++;
            if (col != width-1 && board->data[row*width + col+1] == kTileTypeCoveredBomb) mineCount++;

            if (row != height - 1) {
                if (col != 0 &&       board->data[(row+1)*width + col-1] == kTileTypeCoveredBomb) mineCount++;
                if (                  board->data[(row+1)*width + col] == kTileTypeCoveredBomb) mineCount++;
                if (col != width-1 && board->data[(row+1)*width + col+1] == kTileTypeCoveredBomb) mineCount++;
            }

            board->data[row*width + col] = mineCount | COVER_TILE_BIT;
        }
    }

    return board;
}

void Board_draw(Board* board) {
    // TODO: text printing doesn't work whatsoever!

    if (board->died) {
        Bdisp_MMPrint(0, 0, (unsigned char*)"you lose!", TEXT_MODE_NORMAL, 0xffffffff, 0, 0, TEXT_COLOR_BLACK, TEXT_COLOR_WHITE, true, 0);
        return;
    }

    if (board->won) {
        Bdisp_MMPrint(0, 0, (unsigned char*)"you win!", TEXT_MODE_NORMAL, 0xffffffff, 0, 0, TEXT_COLOR_BLACK, TEXT_COLOR_WHITE, true, 0);
        return;
    }

    for (int row = 0; row < board->height; row++) {
        for (int col = 0; col < board->width; col++) {
            drawSpriteAtPos(board->data[row*board->width + col], 24*col + board->offsetX, 24*row + board->offsetY);
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
}

void Board_handleKeypress(Board* board, int key) {
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
            Board_flag(board);
            break;

        case KEY_CTRL_F2:
            Board_revealSingleTile(board, board->row, board->col);
            break;
    }
}

void Board_flag(Board* board) {
    char* cell = Board_currentCell(board);

    if (!(*cell & COVER_TILE_BIT)) {
        // you shouldnt be able to flag revealed cells
        return;
    }

    *cell ^= FLAG_TILE_BIT;
}

void Board_revealSingleTile(Board* board, int row, int col) {
    char* cell = &board->data[row*board->width + col];

    // unset flag bit if it is set
    if (*cell & FLAG_TILE_BIT) {
        // *cell ^= FLAG_TILE_BIT;
        return;
    }

    // if this cell is uncovered, check the surrounding flag count
    // if it is equal to the cell value, reveal all tiles around the cell
    if (!(*cell & COVER_TILE_BIT)) {
        board->data[row*board->width + col] &= ~COVER_TILE_BIT; // unset covered bit

        if (*cell == 0) return;

        char flagCount = 0;
        int width = board->width;
        int height = board->height;

        if (row != 0) {
            if (col != 0 &&       board->data[(row-1)*width + col-1] & FLAG_TILE_BIT) flagCount++;
            if (                  board->data[(row-1)*width + col] & FLAG_TILE_BIT) flagCount++;
            if (col != width-1 && board->data[(row-1)*width + col+1] & FLAG_TILE_BIT) flagCount++;
        }

        if (col != 0 &&       board->data[row*width + col-1] & FLAG_TILE_BIT) flagCount++;
        if (col != width-1 && board->data[row*width + col+1] & FLAG_TILE_BIT) flagCount++;

        if (row != height - 1) {
            if (col != 0 &&       board->data[(row+1)*width + col-1] & FLAG_TILE_BIT) flagCount++;
            if (                  board->data[(row+1)*width + col] & FLAG_TILE_BIT) flagCount++;
            if (col != width-1 && board->data[(row+1)*width + col+1] & FLAG_TILE_BIT) flagCount++;
        }

        if (flagCount == *cell) {
            Board_revealSurroundingTiles(board, row, col);
        }

        return;
    }

    *cell &= ~COVER_TILE_BIT; // unset covered bit

    if (*cell == kTileTypeZero) {
        // reveal surrounding tiles
        Board_revealSurroundingTiles(board, board->row, board->col);
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

// will reveal all surrounding tiles that aren't flagged
void Board_revealSurroundingTiles(Board* board, int row, int col) {
    int width = board->width;
    int height = board->height;

    if (row != 0) {
        if (col != 0 &&       !(board->data[(row-1)*width + col-1] & FLAG_TILE_BIT)) { Board_revealSingleTile(board, row - 1, col - 1); }
        if (                  !(board->data[(row-1)*width + col] & FLAG_TILE_BIT)) { Board_revealSingleTile(board, row - 1, col); }
        if (col != width-1 && !(board->data[(row-1)*width + col+1] & FLAG_TILE_BIT)) { Board_revealSingleTile(board, row - 1, col + 1); }
    }

    if (col != 0 &&       !(board->data[row*width + col-1] & FLAG_TILE_BIT)) { Board_revealSingleTile(board, row, col - 1); }
    if (col != width-1 && !(board->data[row*width + col+1] & FLAG_TILE_BIT)) { Board_revealSingleTile(board, row, col + 1); }

    if (row != height - 1) {
        if (col != 0 &&       !(board->data[(row+1)*width + col-1] & FLAG_TILE_BIT)) { Board_revealSingleTile(board, row + 1, col - 1); }
        if (                  !(board->data[(row+1)*width + col] & FLAG_TILE_BIT)) { Board_revealSingleTile(board, row + 1, col); }
        if (col != width-1 && !(board->data[(row+1)*width + col+1] & FLAG_TILE_BIT)) { Board_revealSingleTile(board, row + 1, col + 1); }
    }
}

void Board_checkWinCondition(Board* board) {
    // if any tiles are covered still and aren't mines, return
    for (int i = 0; i < board->width * board->height; i++) {
        if (board->data[i] & COVER_TILE_BIT && board->data[i] != kTileTypeCoveredBomb) return;
    }

    // else you win
    board->won = true;
}

char* Board_currentCell(Board* board) {
    return &board->data[board->row*board->width + board->col];
}
