#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "jsmn-master/jsmn.h"
#include "dirent/include/dirent.h"

using namespace std;
string DirContent[100];
string SelectFile;
uint8_t Selected = 0;

int SerchDir(string* NamePointer);

char* FileName = (char*)DirContent[Selected].c_str();
char Buffer[256] = { 0 };

char JSON_STRING[2048] = {0};

static int jsoneq(const char* json, jsmntok_t* tok, const char* s) {
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

int main()
{
    int PatchAmount = SerchDir(&DirContent[0]);
    system("Color 0F");

    while (true)
    {
        system("cls");
        for (int counter = 0; counter < PatchAmount; counter++)
        {
            if (Selected == counter)
            {
                printf("\033[2;37;45m");
            }
            printf(" %d) DirFile: %s\n\r", counter, DirContent[counter].c_str());
            printf("\033[0m");
        }
        printf("\n\r SelectFile: ");

        cin >> SelectFile;

        if (atoi(SelectFile.c_str()))
            Selected = atoi(SelectFile.c_str());
        else
            break;
    }
    FileName = (char*)DirContent[Selected].c_str();
    printf("\n\r");
    FILE* fp = fopen(FileName, "r");
    if (fp)
    {
        for (uint32_t StrCounter = 0; (fgets(Buffer, 256, fp)) != NULL; StrCounter++)
        {
            //printf("%s", Buffer);
            strcat(JSON_STRING, Buffer);
        }
        fclose(fp);
    }
    printf("\n\r\n\r");


    {

        int i;
        int r;
        jsmn_parser p;
        jsmntok_t t[128]; /* We expect no more than 128 tokens */

        jsmn_init(&p);
        r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t,
            sizeof(t) / sizeof(t[0]));
        if (r < 0) {
            printf("Failed to parse JSON: %d\n", r);
            return 1;
        }

        /* Assume the top-level element is an object */
        if (r < 1 || t[0].type != JSMN_OBJECT) {
            printf("Object expected\n");
            return 1;
        }

        /* Loop over all keys of the root object */
        for (i = 1; i < r; i++) {
            if (jsoneq(JSON_STRING, &t[i], "user") == 0) {
                /* We may use strndup() to fetch string value */
                printf("- User: %.*s\n", t[i + 1].end - t[i + 1].start,
                    JSON_STRING + t[i + 1].start);
                i++;
            }
            else if (jsoneq(JSON_STRING, &t[i], "admin") == 0) {
                /* We may additionally check if the value is either "true" or "false" */
                printf("- Admin: %.*s\n", t[i + 1].end - t[i + 1].start,
                    JSON_STRING + t[i + 1].start);
                i++;
            }
            else if (jsoneq(JSON_STRING, &t[i], "uid") == 0) {
                /* We may want to do strtol() here to get numeric value */
                printf("- UID: %.*s\n", t[i + 1].end - t[i + 1].start,
                    JSON_STRING + t[i + 1].start);
                i++;
            }
            else if (jsoneq(JSON_STRING, &t[i], "groups") == 0) {
                int j;
                printf("- Groups:\n");
                if (t[i + 1].type != JSMN_ARRAY) {
                    continue; /* We expect groups to be an array of strings */
                }
                for (j = 0; j < t[i + 1].size; j++) {
                    jsmntok_t* g = &t[i + j + 2];
                    printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
                }
                i += t[i + 1].size + 1;
            }
            else {
                printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
                    JSON_STRING + t[i].start);
            }
        }
        return EXIT_SUCCESS;
    }
    
    return 0;
}

int SerchDir(string* NamePointer)
{
    char PatchName[100] = { 0 };
    short PatchCounter = -1;
    DIR* Patch;
    struct dirent* Dir;
    Patch = opendir(".");

    if (Patch)
    {
        PatchCounter = 0;
        while ((Dir = readdir(Patch)) != NULL)
        {
            *NamePointer = string(Dir->d_name);
            NamePointer++;
            PatchCounter++;
        }
        closedir(Patch);
    }

    return PatchCounter;
}