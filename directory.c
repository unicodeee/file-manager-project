#include "filesystem.h"

#include <string.h>

void initDir(int sector_number, int back_sector)
{
    union block *block = &sector[sector_number];

    memset(block, 0, sizeof(*block));
    block->directory.back = back_sector;
    block->directory.frwd = sector_number == 0 ? -1 : 0;
    block->directory.free = NUM_DIRECTORY_ENTRIES;

    for (int i = 0; i < NUM_DIRECTORY_ENTRIES; ++i) {
        block->directory.dir[i].type = TYPE_FREE;
    }
}

int findEntry(int directory_head_sector, const char *name, int *entry_sector,
              int *entry_index)
{   
    int current_sector =  directory_head_sector;

    while (1) {
        DirectoryEntry *entry;
        for (int i = 0; i < NUM_DIRECTORY_ENTRIES; ++i) {
            entry = &sector[current_sector].directory.dir[i];
            if (entry->type != TYPE_FREE && strcasecmp(entry->name, name) == 0){ // if match
                *entry_sector = current_sector;
                *entry_index = i;

                return 1;
            }
        }

        int next_sector = sector[current_sector].directory.frwd;

        if (next_sector == 0) {break;}
        current_sector = next_sector;
    }
    return 0;
}


int findAvailableEntryInDirectory(int current_directory, DirectoryEntry *entry) {
    for (int i = 0; i < NUM_DIRECTORY_ENTRIES; ++i) {
        if (strcasecmp(sector[current_directory].directory.dir[i].type, TYPE_FREE) == 0) {
            entry = &sector[current_directory].directory.dir[i];
            return 0;
        }
    }
    return 1; // none available in current sector
}

bool createObject(char type, const char *name)
{   
    size_t name_length = strlen(name);
    if (name_length > MAX_NAME_LENGTH) {
        printf("Error: object name is too long: %zu\n", name_length);
        return 0;
    }

    DirectoryEntry *entry;
    if (findAvailableEntryInDirectory(CURRENT_DIRECTORY, entry) != 0) {
        return false;
    }
    switch (type){
        case TYPE_DIRECTORY:
            entry->type = TYPE_DIRECTORY;
            strcpy(entry->name, name);
            entry->link = 0;
            entry->size = 0;          
            break;
        case TYPE_USER_FILE:
            entry->size = TYPE_USER_FILE;
            strcpy(entry->name, name);
            entry->link = 0; //file is empty and has no allocated data sector.
            entry->size = 0;   
            break;
    }
    return false; /* TODO(Member 1): implement CREATE U and CREATE D. */
}

bool deleteObject(const char *name)
{
    (void)name;
    return false; /* TODO(Members 1 and 2): coordinate entry and sector cleanup. */
}

void listDirectory(void)
{
    /* TODO(Member 1): print occupied entries across the directory chain. */
}

bool changeDirectory(const char *name)
{
    (void)name;
    return false; /* TODO(Member 1): implement CD. */
}

void goRoot(void)
{
    CURRENT_DIRECTORY = 0;
}
