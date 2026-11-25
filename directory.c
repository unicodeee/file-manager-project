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

int findEntry(int directory_sector, const char *name, int *entry_sector,
              int *entry_index)
{
    (void)directory_sector;
    (void)name;
    (void)entry_sector;
    (void)entry_index;
    return 0; /* TODO(Member 1): search all sectors in the directory chain. */
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
    current_directory = 0;
}
