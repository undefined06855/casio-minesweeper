#include "scores.h"
#include "save.h"
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>
#include <fxcg/misc.h>
#include <fxcg/file.h>
#include <string.h>

void Scores_create(Scores* scores) {
    scores->notification = 0x0;
}

void Scores_free(Scores* scores) {
    sys_free(scores);
}

void Scores_draw(Scores* scores) {
    int count = Save_getCount();
    unsigned char buf[12];

    for (int i = 0; i < count; i++) {
        Score* score = Save_getAtIndex(i);

        Utils_clearAndFillBuffer(buf, score->time);

        locate_OS(1, i+1);
        Print_OS((const char*)score->name, 0, 0);
        Print_OS(" ", 0, 0);
        Print_OS((const char*)buf, 0, 0);
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
        case KEY_PRGM_F6: {
            Save_reset();
            Save_save();
        } break;

        case KEY_PRGM_5: {
            int len;
            if (MCSGetDlen2((unsigned char*)"Minesweeper", (unsigned char*)"scores", &len) != MCS_SUCCESS) {
                // file doesnt exist yet
                scores->notification = "MainMem file does not exist!";
                break;
            }

            byte* buf = sys_malloc(len);

            if (MCSGetData1(0, len, buf) != MCS_SUCCESS) {
                // uhhhh
                scores->notification = "Failed to read MainMem file data!";
                break;
            }

            // copy buffer to file for debugging
            const char* filePath = "\\fls0\\debug.bin";
            int filePathSize = 15;
            size_t len2 = len;

            unsigned short doubleSizeFileName[filePathSize];
            Bfile_StrToName_ncpy(doubleSizeFileName, filePath, filePathSize);
            int ret = Bfile_CreateEntry_OS(doubleSizeFileName, 1, &len2);
            if (ret < 0) { scores->notification = "Failed to create file!"; break; }

            int fileHandle = Bfile_OpenFile_OS(doubleSizeFileName, 3, 0);
            if (fileHandle < 0) { scores->notification = "Failed to get file handle!"; break; }
            ret = Bfile_WriteFile_OS(fileHandle, buf, len2);
            if (ret < 0) { scores->notification = "Failed to write file!"; break; }

            sys_free(buf);

            scores->notification = "Saved as debug.bin";
        } break;

        case KEY_PRGM_EXIT: {
            return true;
        }
    }

    return false;
}
