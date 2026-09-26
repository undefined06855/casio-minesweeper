#include "save.h"
#include <fxcg/file.h>
#include <fxcg/heap.h>

Score* Save_data = 0x0;
int Save_count = 0;

#define SAVE_FILE_NAME "\\\\fls0\\sweeper_scores.bin"
#define SAVE_FILE_NAME_LEN sizeof(SAVE_FILE_NAME)

void Save_load() {
    unsigned short name[SAVE_FILE_NAME_LEN];
    Bfile_StrToName_ncpy(name, SAVE_FILE_NAME, SAVE_FILE_NAME_LEN);

    int handle = Bfile_OpenFile_OS(name, READ, 0);
    int len = Bfile_GetFileSize_OS(handle);

    if (handle < 0 || len <= 0) {
        return;
    }

    Save_data = sys_malloc(len);
    Save_count = len / sizeof(Score);

    Bfile_ReadFile_OS(handle, &Save_data, len, -1);
    Bfile_CloseFile_OS(handle);
}

void Save_unload() {
    if (Save_data == 0x0) return;
    sys_free(Save_data);
    Save_data = 0x0;
}

void Save_save() {
    if (Save_data == 0x0) return;

    unsigned short name[SAVE_FILE_NAME_LEN];
    Bfile_StrToName_ncpy(name, SAVE_FILE_NAME, SAVE_FILE_NAME_LEN);

    Bfile_DeleteEntry(name);

    unsigned int size = Save_count * sizeof(Score);
    Bfile_CreateEntry_OS(name, CREATEMODE_FILE, &size);

    int handle = Bfile_OpenFile_OS(name, WRITE, 0);
    Bfile_WriteFile_OS(handle, Save_data, size);
    Bfile_CloseFile_OS(handle);
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
