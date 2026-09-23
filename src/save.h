#pragma once

typedef struct {
    char name[16];
    int width;
    int height;
    int centiseconds;
} Score;

void Save_load();
void Save_unload();
void Save_save();

void Save_reset();

int Save_getCount();
Score* Save_getAtIndex(int index);

Score* Save_writeScore();
