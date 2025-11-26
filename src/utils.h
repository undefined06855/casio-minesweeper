#pragma once
#include "sprites.h"

#define true 1
#define false 0
#define bool int
#define KEY_PRGM_DEL 44

void Utils_drawSpriteAtPos(enum TileType sprite, int x, int y);

void Utils_initKeyToNumberMap();
int Utils_keyToNumber(int key);

int Utils_rand();
void Utils_srand(int seed);
int Utils_randrange(int min, int max);
