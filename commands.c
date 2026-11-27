#include "filesystem.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMAND_BUFFER_SIZE 1024
#define MAX_TOKENS 16

typedef enum {
    CMD_UNKNOWN,
    CMD_CREATE,
    CMD_DELETE,
    CMD_OPEN,
    CMD_CLOSE,
    CMD_READ,
    CMD_WRITE,
    CMD_SEEK,
    CMD_DIR,
    CMD_CD,
    CMD_ROOT,
    CMD_EXIT,
    CMD_LIST,
} CommandId;

typedef struct {
    const char *word;
    CommandId id;
    const char *usage;
} CommandInfo;

static const CommandInfo COMMANDS[] = {
    {"create", CMD_CREATE, "create <u|d> <name>"},
    {"delete", CMD_DELETE, "delete <name>"},
    {"open", CMD_OPEN, "open <i|o|u> <name>"},
    {"close", CMD_CLOSE, "close"},
    {"read", CMD_READ, "read <bytes>"},
    {"write", CMD_WRITE, "write <bytes> <data>"},
    {"seek", CMD_SEEK, "seek <position>"},
    {"dir", CMD_DIR, "dir"},
    {"cd", CMD_CD, "cd <name>"},
    {"root", CMD_ROOT, "root"},
    {"ls", CMD_LIST, "list"},
    {"exit", CMD_EXIT, "exit"},
};

#define COMMAND_COUNT (sizeof(COMMANDS) / sizeof(COMMANDS[0]))

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

/* Returns the table entry for a command word, or NULL if it is unknown. */
static const CommandInfo *identifyCommand(const char *word)
{
    for (size_t i = 0; i < COMMAND_COUNT; ++i) {
        if (commandEquals(word, COMMANDS[i].word)) {
            return &COMMANDS[i];
        }
    }

    return NULL;
}

static void printUsage(const CommandInfo *command)
{
    printf("Error: usage: %s\n", command->usage);
}

/* A name is 1-8 characters, with no whitespace or control characters. */
static bool parseName(const char *text, const char **name)
{
    const size_t length = strlen(text);

    if (length == 0 || length > MAX_NAME_LENGTH) {
        printf("Error: name '%s' must be 1 to %d characters\n", text,
               MAX_NAME_LENGTH);
        return false;
    }

    for (size_t i = 0; i < length; ++i) {
        if (!isgraph((unsigned char)text[i])) {
            printf("Error: name '%s' contains an invalid character\n", text);
            return false;
        }
    }

    *name = text;
    return true;
}

/*
 * Parses a single-letter option such as a file type or open mode. The
 * letter is matched case-insensitively and returned in uppercase so it
 * matches TYPE_USER_FILE, TYPE_DIRECTORY, and the I/O/U open modes.
 */
static bool parseLetter(const char *text, const char *allowed,
                        const char *what, char *letter)
{
    const char upper = (char)toupper((unsigned char)text[0]);

    if (text[0] == '\0' || text[1] != '\0' || strchr(allowed, upper) == NULL) {
        printf("Error: invalid %s '%s'\n", what, text);
        return false;
    }

    *letter = upper;
    return true;
}

/* Parses a non-negative decimal number made only of digits. */
static bool parseCount(const char *text, const char *what, size_t *count)
{
    char *end = NULL;
    unsigned long long value;

    if (text[0] == '\0') {
        printf("Error: missing %s\n", what);
        return false;
    }
    for (const char *p = text; *p != '\0'; ++p) {
        if (!isdigit((unsigned char)*p)) {
            printf("Error: %s '%s' must be a non-negative whole number\n",
                   what, text);
            return false;
        }
    }

    errno = 0;
    value = strtoull(text, &end, 10);
    if (errno == ERANGE || *end != '\0' || value > (unsigned long long)INT_MAX) {
        printf("Error: %s '%s' is too large\n", what, text);
        return false;
    }

    *count = (size_t)value;
    return true;
}

/*
 * Runs one tokenized command. argv[0] is the command word. Returns false
 * only when the command is EXIT; parse errors print one "Error:" line and
 * return true so the session continues. Errors from the directory and
 * storage modules are reported by those modules.
 */
static bool executeCommand(int argc, char *argv[])
{
    const CommandInfo *command;
    const char *name = NULL;
    char letter = '\0';
    size_t count = 0;

    if (argc == 0) {
        return true;
    }

    command = identifyCommand(argv[0]);
    if (command == NULL) {
        printf("Error: unknown command '%s'\n", argv[0]);
        return true;
    }

    switch (command->id) {

    case CMD_LIST:
        if (argc != 1) {
            printUsage(command);
        } 
        // list objects
        for (int i = 0; i < 25; i++) {   // TO DO: change to print name only to match ls usecase.
            DirectoryEntry entry = sector[CURRENT_DIRECTORY].directory.dir[i];
            printf("%d\t|%c|%s|%d|%d|\n", i, entry.type, entry.name, entry.link, entry.size);
        }
        break;

    case CMD_CREATE:
        if (argc != 3) {
            printUsage(command);
        } else if (parseLetter(argv[1], "UD", "type", &letter) &&
                   parseName(argv[2], &name)) {
            createObject(letter, name);
        }
        break;

    case CMD_DELETE:
        if (argc != 2) {
            printUsage(command);
        } else if (parseName(argv[1], &name)) {
            deleteObject(name);
        }
        break;

    case CMD_OPEN:
        if (argc != 3) {
            printUsage(command);
        } else if (parseLetter(argv[1], "IOU", "mode", &letter) &&
                   parseName(argv[2], &name)) {
            openFile(letter, name);
        }
        break;

    case CMD_CLOSE:
        if (argc != 1) {
            printUsage(command);
        } else {
            closeFile();
        }
        break;

    case CMD_READ:
        if (argc != 2) {
            printUsage(command);
        } else if (parseCount(argv[1], "byte count", &count)) {
            if (count == 0) {
                puts("Error: byte count must be greater than 0");
            } else {
                readFile(count);
            }
        }
        break;

    case CMD_WRITE:
        /* TODO(Member 3): parse WRITE while preserving spaces in the data. */
        puts("Error: write is not implemented yet");
        break;

    case CMD_SEEK:
        if (argc != 2) {
            printUsage(command);
        } else if (parseCount(argv[1], "position", &count)) {
            seekFile(count);
        }
        break;

    case CMD_DIR:
        if (argc != 1) {
            printUsage(command);
        } else {
            listDirectory();
        }
        break;

    case CMD_CD:
        if (argc != 2) {
            printUsage(command);
        } else if (parseName(argv[1], &name)) {
            changeDirectory(name);
        }
        break;

    case CMD_ROOT:
        if (argc != 1) {
            printUsage(command);
        } else {
            goRoot();
        }
        break;

    case CMD_EXIT:
        if (argc != 1) {
            printUsage(command);
            return true;
        }
        return false;

    case CMD_UNKNOWN:
        break;
    }

    return true;
}

/* Splits a line on spaces and tabs in place. Returns -1 if too many tokens. */
static int tokenize(char *line, char *tokens[], int max_tokens)
{
    int count = 0;
    char *p = line;

    for (;;) {
        while (*p == ' ' || *p == '\t') {
            ++p;
        }
        if (*p == '\0') {
            return count;
        }
        if (count == max_tokens) {
            return -1;
        }

        tokens[count++] = p;
        while (*p != '\0' && *p != ' ' && *p != '\t') {
            ++p;
        }
        if (*p != '\0') {
            *p++ = '\0';
        }
    }
}

bool runCommandArgs(int argc, char *argv[])
{
    return executeCommand(argc, argv);
}

void runCommandLoop(void)
{
    char input[COMMAND_BUFFER_SIZE];
    char *tokens[MAX_TOKENS];

    for (;;) {
        int count;

        fputs("project> ", stdout);
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            putchar('\n');
            return;
        }

        /* A line longer than the buffer: discard the rest and reject it. */
        if (strchr(input, '\n') == NULL && !feof(stdin)) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {
            }
            printf("Error: command longer than %d characters\n",
                   COMMAND_BUFFER_SIZE - 2);
            continue;
        }

        input[strcspn(input, "\r\n")] = '\0';

        count = tokenize(input, tokens, MAX_TOKENS);
        if (count < 0) {
            puts("Error: too many arguments");
            continue;
        }
        if (!executeCommand(count, tokens)) {
            return;
        }
    }
}
