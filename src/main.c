#include "board.h"
#include "utils.h"
#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>
#include <fxcg/system.h>

int main() {
    Bdisp_AllClr_VRAM();

    Bdisp_EnableColor(true);

    Board* board = Board_create(LCD_WIDTH_PX / 24, (LCD_HEIGHT_PX - 24) / 24, 8);

    while (true) {
        // Bdisp_AllClr_VRAM();

        Board_draw(board);

        int key;
        GetKey(&key);
        Board_handleKeypress(board, key);

        Bdisp_PutDisp_DD();
    }

    sys_free(board);

    return 0;
}
