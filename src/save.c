#include "save.h"
#include "utils.h"
#include <fxcg/file.h>
#include <fxcg/heap.h>
#include <string.h>
#include <stdint.h>

#define RECORD_NAME_LEN 16
#define RECORD_SIZE (RECORD_NAME_LEN + 1 + 4)

static void write_fixed_name(char* dst16, const char* src) {
    // Zero-pad and ensure at least one terminator within 16 bytes
    memset(dst16, 0, RECORD_NAME_LEN);
    if (src) {
        strncpy(dst16, src, RECORD_NAME_LEN - 1);
    }
}

Score* first;

Score* Score_loadFromData(char* buffer, int* offset, Score* previous) {
    Score* score = sys_malloc(sizeof(Score));

    score->name = sys_malloc(RECORD_NAME_LEN);
    memcpy(score->name, &buffer[*offset], RECORD_NAME_LEN);
    score->name[RECORD_NAME_LEN - 1] = '\0';

    score->size = buffer[*offset + RECORD_NAME_LEN];

    int time = 0;
    memcpy(&time, &buffer[*offset + RECORD_NAME_LEN + 1], sizeof(int));
    score->time = (int)time;

    score->next = 0x0;

    if (previous != 0x0) {
        previous->next = (struct Score*)score;
    }

    *offset += RECORD_SIZE;

    return score;
}

// returns the next score so it can be chained
Score* Score_saveToBuffer(Score* score, char* buffer, int* offset) {
    write_fixed_name(&buffer[*offset], score->name);
    buffer[*offset + RECORD_NAME_LEN] = score->size;

    int time = (int)score->time;
    memcpy(&buffer[*offset + RECORD_NAME_LEN + 1], &time, sizeof(int));

    *offset += RECORD_SIZE;

    return (Score*)score->next;
}

void Save_load() {
    // MCSDelVar2((unsigned char*)"Minesweeper", (unsigned char*)"scores");

    first = 0x0;

    int len;
    if (MCSGetDlen2((unsigned char*)"Minesweeper", (unsigned char*)"scores", &len) != MCS_SUCCESS || len <= 0) {
        // file doesnt exist yet
        return;
    }

    char* buf = sys_malloc(len);
    if (!buf) return;

    if (MCSGetData1(0, len, buf) != MCS_SUCCESS) {
        sys_free(buf);
        return;
    }

    // parse buffer
    int offset = 0;
    Score* prev = 0x0;
    while (offset + RECORD_SIZE <= len) {
        prev = Score_loadFromData(buf, &offset, prev);
        if (first == 0x0) first = prev;
    }

    sys_free(buf);
}

void Save_save() {
    int count = Save_getCount();
    if (count <= 0) {
        MCS_CreateDirectory((unsigned char*)"Minesweeper");
        MCSDelVar2((unsigned char*)"Minesweeper", (unsigned char*)"scores");
        return;
    }

    int size = count * RECORD_SIZE;

    char* buffer = sys_malloc(size);
    if (!buffer) return;

    int offset = 0;
    Score* score = first;
    while (score) {
        Score_saveToBuffer(score, buffer, &offset);
        score = (Score*)score->next;
    }

    MCS_CreateDirectory((unsigned char*)"Minesweeper");
    MCSDelVar2((unsigned char*)"Minesweeper", (unsigned char*)"scores");
    MCSPutVar2((unsigned char*)"Minesweeper", (unsigned char*)"scores", size, buffer);

    sys_free(buffer);
}

void Save_reset() {
    Score* score = first;
    while (score) {
        Score* next = (Score*)score->next;
        if (score->name) sys_free(score->name);
        sys_free(score);
        score = next;
    }
    first = 0x0;
}

int Save_getCount() {
    int count = 0;
    Score* score = first;
    while (score) {
        count++;
        score = (Score*)score->next;
    }

    return count;
}

Score* Save_getAtIndex(int index) {
    Score* score = first;
    for (int i = 0; i < index && score; i++) { score = (Score*)score->next; }
    return score;
}

void Save_writeScore(char size, int time, char* name) {
    Score* score = sys_malloc(sizeof(Score));
    score->name = sys_malloc(RECORD_NAME_LEN);
    write_fixed_name(score->name, name);
    score->size = size;
    score->time = time;
    score->next = (struct Score*)first;

    first = score;
}
