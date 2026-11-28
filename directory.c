#include "filesystem.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>

void initDir(int sector_number, int back_sector)
{
    union block *block = &sector[sector_number];

    memset(block, 0, sizeof(*block));
    block->directory.back = back_sector;
    block->directory.frwd = -1;
    block->directory.free = NUM_DIRECTORY_ENTRIES;

    for (int i = 0; i < NUM_DIRECTORY_ENTRIES; ++i) {
        block->directory.dir[i].type = TYPE_FREE;
    }
}

int findEntry(int directory_head_sector, const char *name, int *entry_sector,
              int *entry_index)
{
    int current_sector = directory_head_sector;

    while (current_sector != -1) {
        for (int i = 0; i < NUM_DIRECTORY_ENTRIES; ++i) {
            DirectoryEntry *entry = &sector[current_sector].directory.dir[i];
            if (entry->type != TYPE_FREE && strcasecmp(entry->name, name) == 0) {
                if (entry_sector != NULL) {
                    *entry_sector = current_sector;
                }
                if (entry_index != NULL) {
                    *entry_index = i;
                }
                return 1;
            }
        }
        current_sector = sector[current_sector].directory.frwd;
    }
    return 0;
}

DirectoryEntry *findAvailableEntryInDirectory(int current_directory)
{
    int curr = current_directory;
    while (curr != -1) {
        for (int i = 0; i < NUM_DIRECTORY_ENTRIES; ++i) {
            DirectoryEntry *pEntry = &sector[curr].directory.dir[i];
            if (pEntry->type == TYPE_FREE) {
                return pEntry;
            }
        }
        curr = sector[curr].directory.frwd;
    }
    return NULL;
}

int extendDirectory(int directory)
{
    int tail = directory;
    while (sector[tail].directory.frwd != -1) {
        tail = sector[tail].directory.frwd;
    }

    int extended_sector = allocateSector();
    if (extended_sector == -1) {
        return -1;
    }

    initDir(extended_sector, tail);
    sector[tail].directory.frwd = extended_sector;
    return extended_sector;
}

static int getSectorForEntry(const DirectoryEntry *target_entry, int head_directory)
{
    int curr = head_directory;
    while (curr != -1) {
        const DirectoryEntry *first = &sector[curr].directory.dir[0];
        const DirectoryEntry *last = &sector[curr].directory.dir[NUM_DIRECTORY_ENTRIES - 1];
        if (target_entry >= first && target_entry <= last) {
            return curr;
        }
        curr = sector[curr].directory.frwd;
    }
    return -1;
}

bool createObject(char type, const char *name, int this_directory)
{
    if (name == NULL || name[0] == '\0') {
        printf("Error: object name cannot be empty.\n");
        return false;
    }

    size_t name_length = strlen(name);
    if (name_length > MAX_NAME_LENGTH) {
        printf("Error: object name is too long: %zu\n", name_length);
        return false;
    }

    int existing_sector = -1;
    int existing_index = -1;
    if (findEntry(this_directory, name, &existing_sector, &existing_index)) {
        printf("Error: '%s' already exists in this directory.\n", name);
        return false;
    }

    DirectoryEntry *entry = findAvailableEntryInDirectory(this_directory);
    if (entry == NULL) {
        int new_sector = extendDirectory(this_directory);
        if (new_sector == -1) {
            printf("Error: unable to extend directory.\n");
            return false;
        }
        entry = findAvailableEntryInDirectory(new_sector);
        if (entry == NULL) {
            return false;
        }
    }

    int link_sector = 0;
    if (type == TYPE_DIRECTORY) {
        link_sector = allocateSector();
        if (link_sector == -1) {
            printf("Error: no free sectors for new directory.\n");
            return false;
        }
        initDir(link_sector, this_directory);
    } else if (type != TYPE_USER_FILE) {
        printf("Error: invalid object type '%c'.\n", type);
        return false;
    }

    memset(entry->name, 0, sizeof(entry->name));
    strcpy(entry->name, name);
    entry->type = type;
    entry->link = link_sector;
    entry->size = 0;

    int owner_sector = getSectorForEntry(entry, this_directory);
    if (owner_sector != -1 && sector[owner_sector].directory.free > 0) {
        sector[owner_sector].directory.free--;
    }

    return true;
}

bool deleteObject(const char *name)
{
    (void)name;
    return false; /* TODO(Members 1 and 2): coordinate entry and sector cleanup. */
}

void listDirectory(int directory)
{
    for (int i = 0; i < NUM_DIRECTORY_ENTRIES; ++i) {
        DirectoryEntry entry = sector[directory].directory.dir[i];
        if (entry.type == TYPE_FREE) {
            continue;
        }
        printf("%d\t|%c|%s|%d|%d|\n", i, entry.type, entry.name, entry.link, entry.size);
    }
}

void listDirectoryChain(void)
{
    int curr = CURRENT_DIRECTORY;
    while (curr != -1) {
        listDirectory(curr);
        curr = sector[curr].directory.frwd;
    }
}

bool changeDirectory(const char *name)
{
    union block *curr = &sector[CURRENT_DIRECTORY];
    
    if (strcmp(name, "..") == 0 && CURRENT_DIRECTORY != 0) {
        CURRENT_DIRECTORY = curr->directory.back;
        return true;
    }

    for (int i = 0; i < NUM_DIRECTORY_ENTRIES; ++i) {
        DirectoryEntry entry = sector[CURRENT_DIRECTORY].directory.dir[i];
        if (entry.type == TYPE_DIRECTORY && strcmp(entry.name, name) == 0) {
            CURRENT_DIRECTORY = entry.link;
            return true;
        }  
    }
    printf("Directory %s doesn't exist.\n", name);
    return true; /* TODO(Member 1): implement CD. */
}

void goRoot(void)
{
    CURRENT_DIRECTORY = 0;
}
