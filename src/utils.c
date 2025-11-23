#include "utils.h"

void drawSpriteAtPos(enum TileType sprite, int x, int y) {
    const color_t* data = getSprite(sprite);

    if (data == 0x0) {
        data = getSprite(kTileTypeBomb); // !!
    }

    VRAM_CopySprite(data, x, y + 24, 24, 24);
}

// oh this was chatgpt by the way just letting you all know
// there is   zero reason to write this myself
// still wondering if the calculator comes with a random number generator that
// could probably be seeded better?

static int rng_state = 1; // seed however you want

int rand() {
    // signed wraparound still works as a pseudo-random generator
    rng_state = rng_state * 1664525 + 1013904223;
    return (rng_state >> 16) & 0x7FFF; // make a positive-ish int
}

void srand(int seed) {
    rng_state = seed ? seed : 1;
}

// returns int in [min, max)  (max NOT included)
int randrange(int min, int max) {
    int range = max - min;
    if (range <= 0) return min; // safety

    int r = rand() % range;
    if (r < 0) r += range;

    return min + r;
}
