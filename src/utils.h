#pragma once
#include "sprites.h"

#define true 1
#define false 0
#define bool int

void drawSpriteAtPos(enum TileType sprite, int x, int y);

int rand();
void srand(int seed);
int randrange(int min, int max);
