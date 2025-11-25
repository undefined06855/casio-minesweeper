#include "menu.h"
#include "utils.h"
#include "keyboard.h"
#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>
#include <fxcg/system.h>
#include <fxcg/rtc.h>

int main() {
    Bdisp_AllClr_VRAM();

    Bdisp_EnableColor(true);

    char col = TEXT_COLOR_WHITE;
    DefineStatusAreaFlags(3, SAF_BATTERY, &col, &col);

    unsigned int discard = 0;
    unsigned int millisecond = 0;
    RTC_GetTime(&discard, &discard, &discard, &millisecond);
    srand(millisecond);

    Menu* menu = sys_malloc(sizeof(Menu));
    Menu_create(menu);

    while (true) {
        Key_update();

        // we basically overwrite all of vram every frame so this probably isnt
        // necessary
        Bdisp_AllClr_VRAM();

        DisplayStatusArea();

        // non-blocking custom impl
        // though we do have to handle menu key ourselves
        bool shouldExit = Menu_handleKeypress(menu, Key_pressed());
        if (shouldExit) break;

        // note all functions will fall through as needed
        // i.e. Menu_draw will call Board_draw if it has a Board*
        Menu_draw(menu);

        Bdisp_PutDisp_DD();
    }

    Menu_free(menu);

    return 0;
}
