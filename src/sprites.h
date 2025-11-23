#pragma once
#include <fxcg/display.h>

static const int COVER_TILE_BIT = 1 << 4;
static const int FLAG_TILE_BIT = 1 << 4;

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
    kTileTypeBomb = 9,
    kTileTypeFlag = 10,

    kTileTypeCoveredBomb = kTileTypeBomb | 1 << 4,
};

const color_t* getSprite(enum TileType sprite);
