#include "filesystem.h"

#include <stdio.h>

union block sector[NUM_SECTORS];
nodeType *free_sector_list = NULL;
int current_directory = 0;
OpenFileState open_file = {false, -1, -1, '\0', 0};

bool verify_layout(void)
{
    if (sizeof(int) != 4 || sizeof(DirectoryEntry) != 20 ||
        sizeof(union block) != 512) {
        fprintf(stderr,
                "Unsupported layout: int=%zu, directory entry=%zu, block=%zu\n",
                sizeof(int), sizeof(DirectoryEntry), sizeof(union block));
        return false;
    }

    return true;
}

int main(void)
{
    if (!verify_layout()) {
        return 1;
    }

    puts("File manager scaffold is ready.");
    return 0;
}
