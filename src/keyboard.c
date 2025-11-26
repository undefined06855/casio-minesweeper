#include "keyboard.h"
#include <fxcg/system.h>
#include <fxcg/keyboard.h>

int lastKey;
int currentKey;

// note: GetKey is blocking!
// using our non-blocking version is a tradeoff since itll use more battery
// though if we really cared about battery we wouldnt be using 16 bit colours..

void Key_update() {
    lastKey = currentKey;
    currentKey = PRGM_GetKey(); // non-blocking impl from sdk

    // TODO: this doesnt work + backlight + auto power off?
    // and also usb but no clue what i should do for that
    switch (Key_pressed()) {
        case KEY_PRGM_MENU: {
            // int _;
            // GetKey(&_);
        } break;
    }
}

int Key_pressed() {
    if (lastKey != currentKey) return currentKey;
    else return 0;
}
