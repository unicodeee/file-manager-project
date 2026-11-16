#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdbool.h>
#include <stddef.h>

#define NUM_SECTORS 100
#define DIRECTORY_ENTRIES 25
#define DATA_SIZE 504
#define MAX_NAME_LENGTH 8

#define TYPE_FREE 'F'
#define TYPE_USER_FILE 'U'
#define TYPE_DIRECTORY 'D'

typedef struct {
    char type;
    char name[MAX_NAME_LENGTH + 1];
    int link;
    short size;
} DirectoryEntry;

union block {
    struct {
        int back;
        int frwd;
        int free;
        DirectoryEntry dir[DIRECTORY_ENTRIES];
    } directory;
    struct {
        int backf;
        int frwdf;
        char userdata[DATA_SIZE];
    } data;
};

typedef struct nodeType {
    int info;
    struct nodeType *link;
} nodeType;

typedef struct {
    bool is_open;
    int directory_sector;
    int entry_index;
    char mode;
    int position;
} OpenFileState;

extern union block sector[NUM_SECTORS];
extern nodeType *free_sector_list;
extern int current_directory;
extern OpenFileState open_file;

bool verify_layout(void);

#endif
