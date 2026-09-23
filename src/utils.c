#include "utils.h"
#include <fxcg/keyboard.h>
#include <fxcg/misc.h>

void Utils_drawSpriteAtPos(enum TileType sprite, int x, int y) {
    const color_t* data = getSprite(sprite);

    if (data == 0x0) {
        data = getSprite(Utils_randrange(kTileTypeZero, kTileTypeMine+1)); // !!
    }

    VRAM_CopySprite(data, x, y + 24, 24, 24);
}

void Utils_clearAndFillBuffer(unsigned char* buffer, int number) {
    for (int i = 0; i < 12; i++) buffer[i] = '\x00';
    itoa(number, buffer);
}

int keyToNumberMap[23];

void Utils_initKeyToNumberMap() {
    keyToNumberMap[KEY_PRGM_0 - 52] = 0;
    keyToNumberMap[KEY_PRGM_1 - 52] = 1;
    keyToNumberMap[KEY_PRGM_2 - 52] = 2;
    keyToNumberMap[KEY_PRGM_3 - 52] = 3;
    keyToNumberMap[KEY_PRGM_4 - 52] = 4;
    keyToNumberMap[KEY_PRGM_5 - 52] = 5;
    keyToNumberMap[KEY_PRGM_6 - 52] = 6;
    keyToNumberMap[KEY_PRGM_7 - 52] = 7;
    keyToNumberMap[KEY_PRGM_8 - 52] = 8;
    keyToNumberMap[KEY_PRGM_9 - 52] = 9;
}

int Utils_keyToNumber(int key) {
    return keyToNumberMap[key - 52];
}

// stdlib.h rand crashed the calculator :heart:

static int rng_state = 1;

int Utils_rand() {
    rng_state = rng_state * 1664525 + 1013904223;
    return (rng_state >> 16) & 0x7FFF;
}

void Utils_srand(int seed) {
    rng_state = seed ? seed : 1;
}

// returns int in [min, max)
int Utils_randrange(int min, int max) {
    int range = max - min;
    if (range <= 0) return min;

    int r = Utils_rand() % range;
    if (r < 0) r += range;

    return min + r;
}
