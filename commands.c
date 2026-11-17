#include "filesystem.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define COMMAND_BUFFER_SIZE 1024

static bool commandEquals(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0') {
        if (tolower((unsigned char)*left) != tolower((unsigned char)*right)) {
            return false;
        }
        ++left;
        ++right;
    }

    return *left == '\0' && *right == '\0';
}

bool runCommandArgs(int argc, char *argv[])
{
    if (argc == 1 && commandEquals(argv[0], "exit")) {
        return false;
    }

    fputs("Command parser scaffold received:", stdout);
    for (int i = 0; i < argc; ++i) {
        printf(" %s", argv[i]);
    }
    putchar('\n');
    puts("Command execution is not implemented yet.");
    return true;
}

void runCommandLoop(void)
{
    char input[COMMAND_BUFFER_SIZE];

    for (;;) {
        fputs("project> ", stdout);
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            putchar('\n');
            return;
        }

        input[strcspn(input, "\n")] = '\0';
        if (commandEquals(input, "exit")) {
            return;
        }
        if (input[0] != '\0') {
            puts("Command parser scaffold: command not implemented yet.");
        }
    }
}
