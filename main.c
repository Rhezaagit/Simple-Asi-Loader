//Written By Rhezaagit

#include <processthreadsapi.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdio.h>
#include "vorbis_data.h"

char** IgnoredModList = NULL;
unsigned int IgnoredListModSize = 0;

void AddIgnoredMod(const char* str) {
    char** NewIgnoredModList = realloc(IgnoredModList, IgnoredListModSize * sizeof(char*));
    NewIgnoredModList[IgnoredListModSize] = strdup(str);
    IgnoredListModSize++;
    IgnoredModList = NewIgnoredModList;
}

void LoadSettings(const char* FilePath) {
    FILE* f = fopen(FilePath, "r");
    if (!f) {
        f = fopen("./SimpleAsiLoader.ini", "w");
        fwrite(NULL, 0, 0, f);

        fclose(f);
        return;
    }

    unsigned int size = 0;
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);

    char* Buf = malloc(size + 1);
    fread(Buf, 1, size, f);
    size[Buf] = '\0'; // What

    char word[256] = {0};
    BOOL isignore = FALSE;
    for (unsigned int i = 0, p = 0; i < size; i++) {
        const char c = i[Buf]; // Who Made This?

        if (c == ' ' || c == '\n' || c == '\0') {
            if (strcmp(word, "[ignore]") == 0) {
                isignore = !isignore;
            }

            else if (c == ' ' || c == '\n') {
                AddIgnoredMod(word);
            }

            memset(word, 0, 255);
            p = 0;
        } else p++[word] = i[Buf]; // Nah
    }

    free(Buf);
    fclose(f);
}

//log so you can see Log Of the Modloader 
void LogMod(const char* text) {
    FILE* f = fopen("SimpleAsiLoader.log", "a");
    if (f) {
        fprintf(f, "%s\n", text);
        fclose(f);
    }
}

//Iterate Every asi and dll file to run loadlibraryA
void LoadAsi(const char* folderPath) {
    WIN32_FIND_DATA data;

    char paths[256] = {0}, Logs[256] = {0};
    sprintf(paths, "%s/*.asi", folderPath);
    HANDLE HFind = FindFirstFile(paths, &data);

    if (HFind != INVALID_HANDLE_VALUE) {
        do {
            strset(paths, 0);
            sprintf(paths, "%s/%s", folderPath, data.cFileName);

            BOOL finded = FALSE;
            for (unsigned int i = 0; i < IgnoredListModSize; i++) {
                if (strcmp(data.cFileName, IgnoredModList[i]) == 0) {
                    finded = TRUE;
                    break;
                }
            }

            strset(Logs, 0);
            if (!finded) {
                LoadLibraryA(paths) ? 
                    sprintf(Logs, "Success Loading %s", data.cFileName) : 
                    sprintf(Logs, "Failed Loading %s", data.cFileName);

                LogMod(Logs);
            }

        } while (FindNextFile(HFind, &data));
    }

    FindClose(HFind);
}

//Dont Think To Much About This Just create The Original vorbisFileHooked.dll That GTA SA Needed
void CreateOriginal() {
    FILE* check = fopen("vorbisFileHooked.dll", "rb");
    if (check) {
        fclose(check);
        return;    
    }

    FILE* f = fopen("vorbisFileHooked.dll", "wb");
    if (f) {
        fwrite(vorbisHooked_dll, 1, vorbisHooked_dll_len, f);
        fclose(f);
        ExitProcess(0); // the GTA SA still not load the dll yet so you need run it again
    }
}

// Clear The Log File Buffer
void ResetLog() {
	FILE* f = fopen("SimpleAsiLoader.log", "w");
	if (f) {
		fwrite(NULL, 1, 0, f);
        fclose(f);
	}
}

//The Main Function start When The Loader Begin 
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) { 
    if (fdwReason == DLL_PROCESS_ATTACH) {
    	ResetLog();
    
        CreateOriginal();
        LoadSettings("./SimpleAsiLoader.ini");
        LoadAsi("./");
        LoadAsi("./scripts/");
    } else if (fdwReason == DLL_PROCESS_DETACH) {
        if (!IgnoredModList) return TRUE;
        for (unsigned int i = 0; i < IgnoredListModSize; i++) free(IgnoredModList[i]);
        free(IgnoredModList);
    }

    return TRUE;
}
