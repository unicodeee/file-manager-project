#include "filesystem.h"

#include <stdlib.h>

bool buildList(void)
{
    cleanupStorage();

    for (int number = NUM_SECTORS - 1; number >= 1; --number) {
        nodeType *node = malloc(sizeof(*node));
        if (node == NULL) {
            cleanupStorage();
            return false;
        }

        node->info = number;
        node->link = free_sector_list;
        free_sector_list = node;
    }

    return true;
}

int Remove(void)
{
    if (free_sector_list == NULL) {
        return -1;
    }

    nodeType *removed = free_sector_list;
    const int sector_number = removed->info;
    free_sector_list = removed->link;
    free(removed);
    return sector_number;
}

bool Insert(int sector_number)
{
    if (sector_number <= 0 || sector_number >= NUM_SECTORS) {
        return false;
    }

    for (nodeType *node = free_sector_list; node != NULL; node = node->link) {
        if (node->info == sector_number) {
            return false;
        }
    }

    nodeType *node = malloc(sizeof(*node));
    if (node == NULL) {
        return false;
    }

    node->info = sector_number;
    node->link = free_sector_list;
    free_sector_list = node;
    return true;
}

void cleanupStorage(void)
{
    while (free_sector_list != NULL) {
        nodeType *removed = free_sector_list;
        free_sector_list = removed->link;
        free(removed);
    }
}

bool openFile(char mode, const char *name)
{
    (void)mode;
    (void)name;
    return false; /* TODO(Member 2): implement OPEN. */
}

void closeFile(void)
{
    open_file = (OpenFileState){false, -1, -1, '\0', 0};
}

bool readFile(size_t byte_count)
{
    (void)byte_count;
    return false; /* TODO(Member 2): implement READ across sector chains. */
}

bool writeFile(size_t byte_count, const char *data)
{
    (void)byte_count;
    (void)data;
    return false; /* TODO(Member 2): implement WRITE and sector allocation. */
}

bool seekFile(size_t position)
{
    (void)position;
    return false; /* TODO(Member 2): implement SEEK. */
}
