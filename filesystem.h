#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdbool.h>
#include <stddef.h>

#define NUM_SECTORS 100
#define NUM_DIRECTORY_ENTRIES 25
#define DATA_SIZE 504
#define MAX_NAME_LENGTH 8

#define TYPE_FREE 'F'
#define TYPE_USER_FILE 'U'
#define TYPE_DIRECTORY 'D'

typedef struct {
    char type; // 1
    char name[MAX_NAME_LENGTH + 1]; // 9
    int link; // 4
    short size; // 2
} DirectoryEntry; // 16 + 4 padding = 20

union block {
    struct {
        int back;
        int frwd;
        int free;
        DirectoryEntry dir[NUM_DIRECTORY_ENTRIES];
    } directory;
    struct {
        int backf;
        int frwdf;
        char userdata[DATA_SIZE];
    } data;
};

typedef struct nodeType {
    int info;               // sector number
    struct nodeType *link;  // next free-sector node
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
extern int CURRENT_DIRECTORY;
extern OpenFileState open_file;

bool verify_layout(void);
bool initDisk(void);
void initDir(int sector_number, int back_sector);

// directory.c
int findEntry(int directory_sector, const char *name, int *entry_sector,
              int *entry_index);
bool createObject(char type, const char *name);
bool deleteObject(const char *name);
void listDirectory(void);
bool changeDirectory(const char *name);
void goRoot(void);

bool buildList(void);
int Remove(void);
bool Insert(int sector_number);
int allocateSector(void);
bool releaseSector(int sector_number);
bool releaseFileChain(int first_sector);
bool releaseDirectoryChain(int first_sector);
void cleanupStorage(void);

bool openFile(char mode, const char *name);
void closeFile(void);
bool readFile(size_t byte_count);
bool writeFile(size_t byte_count, const char *data);
bool seekFile(size_t position);

// commands.c
bool runCommandArgs(int argc, char *argv[]);
void runCommandLoop(void);

#endif
