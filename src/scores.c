#include "scores.h"
#include "save.h"
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>
#include <fxcg/misc.h>
#include <fxcg/file.h>

#define SCORE_CELL_HEIGHT 40

void Scores_create(Scores* scores) {
    scores->notification = 0x0;
    scores->scroll = 0;
}

void Scores_free(Scores* scores) {
    sys_free(scores);
}

void Scores_draw(Scores* scores) {
    int count = Save_getCount();
    unsigned char buf[12];

    int y = scores->scroll;

    for (int i = 0; i < count; i++) {
        Score* score = Save_getAtIndex(i);

        int seconds = score->centiseconds / 10;
        int centiseconds = score->centiseconds % 10;

        // TODO: finish this
        locate_OS(1, i+1);
        Print_OS((const char*)score->name, 0, 0);
        Print_OS(" ", 0, 0);

        Utils_clearAndFillBuffer(buf, seconds);
        Print_OS((const char*)buf, 0, 0);
        Print_OS(".", 0, 0);
        Utils_clearAndFillBuffer(buf, centiseconds);
        Print_OS((const char*)buf, 0, 0);
        Print_OS("s", 0, 0);
    }
}

void Scores_drawStatusArea(Scores* scores) {
    int x = 24;
    int y = 2;

    if (scores->notification) {
        PrintMini(&x, &y, scores->notification, 1 << 6, 0xffffffff, 0, 0, COLOR_RED, COLOR_WHITE, true, 0);
        return;
    }

    unsigned char buf[12];
    Utils_clearAndFillBuffer(buf, Save_getCount());

    PrintMini(&x, &y, (const char*)buf, 1 << 6, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, " scores saved. |  ", 1 << 6, 0xffffffff, 0, 0, COLOR_BLACK, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, "F6", 1 << 6, 0xffffffff, 0, 0, COLOR_NAVY, COLOR_WHITE, true, 0);
    PrintMini(&x, &y, " - Reset!", 1 << 6, 0xffffffff, 0, 0, COLOR_RED, COLOR_WHITE, true, 0);
}

bool Scores_handleKeypress(Scores* scores, int key) {
    switch (key) {
        case KEY_PRGM_UP: {
            scores->scroll -= 30;
            if (scores->scroll < 0) scores->scroll = 0;
        } break;

        case KEY_PRGM_DOWN: {
            int max = SCORE_CELL_HEIGHT * Save_getCount() - LCD_HEIGHT_PX;
            scores->scroll += 30;
            if (scores->scroll < max) scores->scroll = max;
        } break;

        case KEY_PRGM_F6: {
            Save_reset();
            Save_save();
        } break;

        case KEY_PRGM_EXIT: {
            return true;
        }
    }

    return false;
}
