#include "utils.h"
#include <fxcg/keyboard.h>

void Utils_drawSpriteAtPos(enum TileType sprite, int x, int y) {
    const color_t* data = getSprite(sprite);

    if (data == 0x0) {
        data = getSprite(Utils_randrange(kTileTypeZero, kTileTypeMine+1)); // !!
    }

    VRAM_CopySprite(data, x, y + 24, 24, 24);
}

/*
#define KEY_PRGM_3 52
#define KEY_PRGM_6 53
#define KEY_PRGM_9 54
#define KEY_PRGM_2 62
#define KEY_PRGM_5 63
#define KEY_PRGM_8 64
#define KEY_PRGM_0 71
#define KEY_PRGM_1 72
#define KEY_PRGM_4 73
#define KEY_PRGM_7 74
*/

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

// oh this was chatgpt by the way just letting you all know
// there is   zero reason to write this myself
// still wondering if the calculator comes with a random number generator that
// could probably be seeded better?

static int rng_state = 1; // seed however you want

int Utils_rand() {
    // signed wraparound still works as a pseudo-random generator
    rng_state = rng_state * 1664525 + 1013904223;
    return (rng_state >> 16) & 0x7FFF; // make a positive-ish int
}

void Utils_srand(int seed) {
    rng_state = seed ? seed : 1;
}

// returns int in [min, max)  (max NOT included)
int Utils_randrange(int min, int max) {
    int range = max - min;
    if (range <= 0) return min; // safety

    int r = Utils_rand() % range;
    if (r < 0) r += range;

    return min + r;
}
