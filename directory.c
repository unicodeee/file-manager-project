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

bool createObject(char type, const char *name)
{
    (void)type;
    (void)name;
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
