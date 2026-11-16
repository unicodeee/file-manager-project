#include "filesystem.h"

#include <stdio.h>
#include <string.h>

#define COMMAND_BUFFER_SIZE 1024

void runCommandLoop(void)
{
    char input[COMMAND_BUFFER_SIZE];

    for (;;) {
        fputs("fs> ", stdout);
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            putchar('\n');
            return;
        }

        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "EXIT") == 0) {
            return;
        }
        if (input[0] != '\0') {
            puts("Command parser scaffold: command not implemented yet.");
        }
    }
}
