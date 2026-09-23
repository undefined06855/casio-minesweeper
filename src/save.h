#pragma once
#include "utils.h"

struct Score;

typedef struct {
    char* name;
    char size;
    int time;
    struct Score* next;
} Score;

Score* Score_loadFromData(byte* buffer, int* offset, Score* previous);
Score* Score_saveToBuffer(Score* score, byte* buffer, int* offset);

void Save_load();
void Save_save();

void Save_reset();

int Save_getCount();
Score* Save_getAtIndex(int index);

void Save_writeScore(char size, int time, char* name);
