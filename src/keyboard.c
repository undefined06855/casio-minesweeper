#include "keyboard.h"
#include <fxcg/system.h>
#include <fxcg/keyboard.h>

int lastKey;
int currentKey;

// note: GetKey is blocking!
// using our non-blocking version is a tradeoff since itll use more battery
// though if we really cared about battery we wouldnt be using 16 bit colours..

void Key_update() {
    // handle the MENU button... though this doesn't seem to work!
    // int _; int __; unsigned short ___;
    // GetKeyWait_OS(&_, &__, KEYWAIT_HALTON_TIMERON, 0, false, &___);

    lastKey = currentKey;
    currentKey = PRGM_GetKey(); // non-blocking impl from sdk
}

int Key_pressed() {
    if (lastKey != currentKey) return currentKey;
    else return 0;
}

int Key_currentlyPressed() {
    return currentKey;
}
