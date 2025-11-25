#include "menu.h"
#include "utils.h"
#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>
#include <fxcg/system.h>
#include <fxcg/rtc.h>

int main() {
    Bdisp_AllClr_VRAM();

    Bdisp_EnableColor(true);

    unsigned int discard = 0;
    unsigned int millisecond = 0;
    RTC_GetTime(&discard, &discard, &discard, &millisecond);
    srand(millisecond);

    // note all functions will fall through as needed
    // i.e. Menu_draw will call Board_draw if it has a Board*

    Menu* menu = sys_malloc(sizeof(Menu));
    Menu_create(menu);

    while (true) {
        Menu_draw(menu);

        int key;
        GetKey(&key);
        Menu_handleKeypress(menu, key);

        Bdisp_PutDisp_DD();
    }

    Menu_free(menu);

    return 0;
}
