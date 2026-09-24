#pragma once
#include "sprites.h"
#include <stdbool.h>

#define KEY_PRGM_DEL 44
#define MCS_SUCCESS 0

typedef unsigned char byte;

void Utils_drawSpriteAtPos(TileType sprite, int x, int y);

void Utils_clearAndFillBuffer(unsigned char* buffer, int number);

void Utils_initKeyToNumberMap();
int Utils_keyToNumber(int key);

int Utils_rand();
void Utils_srand(int seed);
int Utils_randrange(int min, int max);
