#include "sprites.h"
#include "spritedata.h"

const color_t* sprites[MAX_SPRITE_COUNT] = {
    // 0:
    spr_zero,
    spr_one,
    spr_two,
    spr_three,
    spr_four,
    spr_five,
    spr_six,
    spr_seven,
    spr_eight,
    spr_mine,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    // 16:
    spr_covered, // set bit 1 << 4 to cover the tile
    spr_covered,
    spr_covered,
    spr_covered,
    spr_covered,
    spr_covered,
    spr_covered,
    spr_covered,
    spr_covered,
    spr_covered,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    // 32:
    0x0, // set bit 1 << 5 to flag the tile
    0x0, // but these are the uncovered tiles, which cant be flagged!
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    // 48:
    spr_flag, // these are the covered tiles, which can be flagged
    spr_flag,
    spr_flag,
    spr_flag,
    spr_flag,
    spr_flag,
    spr_flag,
    spr_flag,
    spr_flag,
    spr_flag,
    0x0,
    0x0,
    0x0,
    0x0,
    spr_hitMine,
    spr_flaggedWrong
};

const color_t* getSprite(enum TileType sprite) {
    if (sprite >= MAX_SPRITE_COUNT) return 0x0;

    return sprites[sprite];
}

#undef MAX_SPRITE_COUNT
