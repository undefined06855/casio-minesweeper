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

    unsigned int _ = 0;
    unsigned int millisecond = 0;
    RTC_GetTime(&_, &_, &_, &millisecond);
    Utils_srand(millisecond);

    Utils_initKeyToNumberMap();


    Menu* menu = sys_malloc(sizeof(Menu));
    Menu_create(menu);

    while (true) {
        Key_update();

        // we basically overwrite all of vram every frame so this probably isnt
        // necessary
        Bdisp_AllClr_VRAM();

        Menu_handleKeypress(menu, Key_pressed());

        // note all functions will fall through as needed
        // i.e. Menu_draw will call Board_draw if it has a Board*
        Menu_draw(menu);

        DisplayStatusArea();

        Menu_drawStatusArea(menu);

        Bdisp_PutDisp_DD();

        // TODO: make this better
        if (Key_pressed() == KEY_PRGM_MENU) break;
    }

    Menu_free(menu);

    return 0;
}
