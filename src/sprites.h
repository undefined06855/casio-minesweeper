#pragma once
#include <fxcg/display.h>

#define MAX_SPRITE_COUNT 64

static const int COVER_TILE_BIT = 1 << 4;
static const int FLAG_TILE_BIT = 1 << 5;

enum TileType {
    kTileTypeZero = 0,
    kTileTypeOne = 1,
    kTileTypeTwo = 2,
    kTileTypeThree = 3,
    kTileTypeFour = 4,
    kTileTypeFive = 5,
    kTileTypeSix = 6,
    kTileTypeSeven = 7,
    kTileTypeEight = 8,
    kTileTypeMine = 9,

    kTileTypeIncorrectFlag = MAX_SPRITE_COUNT - 1,
    kTileTypeHitMine = MAX_SPRITE_COUNT - 2
};

const color_t* getSprite(enum TileType sprite);
