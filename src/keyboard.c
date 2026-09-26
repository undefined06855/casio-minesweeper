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
}

int Key_pressed() {
    if (lastKey != currentKey) return currentKey;
    else return 0;
}

int Key_currentlyPressed() {
    return currentKey;
}

// thanks to parisse on the cemetech forums

int menuTimer = 0;
void Key_sendMenuKey(){
    Timer_Stop(menuTimer);
    Timer_Deinstall(menuTimer);
    menuTimer = 0;

    // OS_InnerWait_ms(50);
    // menu row 9 col 4
    // Keyboard_PutKeycode(4,9,KEY_CTRL_MENU);
    // keycode is ignored if arg1 and arg2 are >0
#ifdef MPM
    Keyboard_PutKeycode(6, 10, 0);
#else
    // Keyboard_PutKeycode(4, 9, 0);
    Keyboard_PutKeycode(-1, -1, KEY_CTRL_MENU);
#endif
}

void Key_simulateMenuPress() {
    if (menuTimer) return;
    menuTimer = Timer_Install(0, Key_sendMenuKey, 10);
    Timer_Start(menuTimer);

    int _;
    GetKey(&_);
}
