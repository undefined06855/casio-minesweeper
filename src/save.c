#include "save.h"
#include "utils.h"
#include <fxcg/file.h>
#include <fxcg/heap.h>

Score* Save_data = 0x0;
int Save_count = 0;

void Save_load() {
    // MCSDelVar2(SAVE_DIR, SAVE_FILE);

    int len;
    if (MCSGetDlen2(SAVE_DIR, SAVE_FILE, &len) != MCS_SUCCESS || len <= 0) {
        // file doesnt exist yet
        return;
    }

    Save_data = sys_malloc(len);
    Save_count = len / sizeof(Score);
    if (!Save_data) return;

    if (MCSGetData1(0, len, Save_data) != MCS_SUCCESS) {
        sys_free(Save_data);
        Save_data = 0x0;
        Save_count = 0;
        return;
    }
}

void Save_unload() {
    if (Save_data == 0x0) return;
    sys_free(Save_data);
}

void Save_save() {
    if (Save_data == 0x0) return;

    int count = Save_getCount();
    if (count == 0) {
        MCS_CreateDirectory(SAVE_DIR);
        MCSDelVar2(SAVE_DIR, SAVE_FILE);
        return;
    }

    MCS_CreateDirectory(SAVE_DIR);
    MCSDelVar2(SAVE_DIR, SAVE_FILE);
    MCSPutVar2(SAVE_DIR, SAVE_FILE, Save_count * sizeof(Score), Save_data);
}

void Save_reset() {
    if (Save_data == 0x0) return;
    Save_count = 0;
}

int Save_getCount() {
    if (Save_data == 0x0) return 0;
    return Save_count;
}

Score* Save_getAtIndex(int index) {
    if (index >= Save_count) return 0x0;
    return &Save_data[index];
}

Score* Save_writeScore() {
    if (Save_count >= 200) return 0x0;

    Save_count++;
    Save_data = sys_realloc(Save_data, Save_count * sizeof(Score));

    return Save_getAtIndex(Save_count - 1);
}
