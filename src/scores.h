#pragma once
#include <stdbool.h>

typedef struct {
    const char* notification;
    int scroll;
} Scores;

void Scores_create(Scores* scores);
void Scores_free(Scores* scores);

void Scores_draw(Scores* scores);
void Scores_drawStatusArea(Scores* scores);
bool Scores_handleKeypress(Scores* scores, int key);
