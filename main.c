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
                "Unsupported memory layout:\n"
                "expected int=4, directory entry=20, block=512\n"
                "\nint=%zu, directory entry=%zu, block=%zu\n",
                sizeof(int), sizeof(DirectoryEntry), sizeof(union block));
        return false;
    }

    return true;
}

bool initDisk(void)
{
    initDir(0, -1);
    current_directory = 0;
    open_file = (OpenFileState){false, -1, -1, '\0', 0};
    return buildList();
}

int main(int argc, char *argv[])
{
    bool keep_running = true;

    if (!verify_layout()) {
        return 1;
    }

    if (!initDisk()) {
        fputs("Unable to initialize the simulated disk.\n", stderr);
        cleanupStorage();
        return 1;
    }

    puts("Simplified file system ready. Enter exit to quit.");
    if (argc > 1) {
        keep_running = runCommandArgs(argc - 1, &argv[1]);
    }
    if (keep_running) {
        runCommandLoop();
    }

    cleanupStorage();
    return 0;
}
