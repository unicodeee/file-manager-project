# Simplified File Manager

A CS 149 group project that simulates a 100-sector file system entirely in
memory. Each sector is exactly 512 bytes and is represented by `union block`.
The program will support regular files, subdirectories, a linked list of free
sectors, and the commands `CREATE`, `DELETE`, `OPEN`, `CLOSE`, `READ`, `WRITE`,
`SEEK`, `DIR`, `CD`, `ROOT`, and `EXIT`.

## Build and run

The target environment is Linux with a C11 compiler.

```sh
make
./project create u A
```

Use `make clean` to remove generated files. The current program is a compiling
scaffold: initialization and free-list basics work, while unfinished operations
are marked with `TODO(Member N)` comments.

## Command convention

Start the program with one command after `./project`. Use lowercase command,
type, and mode arguments; file and directory names may use uppercase letters.
For example, this starts the in-memory file system and submits a request to
create a regular file named `A`:

```sh
./project create u A
```

The startup command is processed before the interactive prompt appears, so the
same in-memory file system remains available for additional commands. Running
`./project` without arguments starts directly at the `project>` prompt.

| Operation | Convention |
| --- | --- |
| Create a regular file | `./project create u A` |
| Create a directory | `./project create d DOCS` |
| Delete an entry | `./project delete A` |
| Open for input/output/update | `./project open i A`, `./project open o A`, or `./project open u A` |
| Close the open file | `./project close` |
| Read bytes | `./project read 100` |
| Write bytes | `./project write 5 HELLO` |
| Seek | `./project seek 600` |
| List the current directory | `./project dir` |
| Change directory | `./project cd DOCS` |
| Return to root | `./project root` |
| Exit | `./project exit` |

Inside the interactive prompt, use the same convention without the
`./project` prefix, for example `create u A`. Names remain limited to eight
characters. When writing data that contains spaces, quote it in command-line
mode, for example `./project write 6 " WORLD"`.

## Project files

| File | Purpose | Primary owner |
| --- | --- | --- |
| `filesystem.h` | Shared constants, structures, globals, and function prototypes | Member 1 / group approval |
| `directory.c` | Disk/directory initialization and directory operations | Member 1 |
| `storage.c` | Free-sector list and regular-file operations | Member 2 |
| `commands.c` | Command parsing and main command loop | Member 3 |
| `main.c` | Global state, layout checks, startup, and shutdown | Shared |
| `Makefile` | Linux build commands | Shared |

## Shared design contract

- `sector[100]` is the simulated disk; sector `0` is always the root.
- `DirectoryEntry` must be 20 bytes and `union block` must be 512 bytes.
- Names contain at most 8 characters plus the null terminator.
- Types are `F` (free), `U` (regular/user file), and `D` (directory).
- A data sector stores 504 bytes and uses `0` as the end-of-chain link.
- Only one file may be open at a time. Its size and current position are
  separate values.
- The agreed shared entry points are declared in `filesystem.h`. Do not change
  structures, globals, or prototypes without group agreement.
- Errors should be one clear line that starts with `Error:`.

## Task tracker

Target completion: **Wednesday, November 25, 2026**.

Status legend: `❌` not started · `🚧` in progress · `✅` complete

Update the status icon in this table when work starts or finishes. If a due
date changes, update it here so everyone sees the same plan.

### Shared responsibilities

| Status | Task | Owner | Due |
| --- | --- | --- | --- |
| ✅ | Agree on structures, globals, prototypes, command syntax, file layout, and error style | All | 2026-11-16 |
| 🚧 | Review the complete code and keep naming, comments, and formatting consistent | All | 2026-11-23 |
| ❌ | Integrate modules and resolve cross-module bugs | All | 2026-11-24 |
| ❌ | Run the final Linux build and complete-system test | All | 2026-11-25 |

### Member 1 — disk and directory management

| Status | Task | Due |
| --- | --- | --- |
| ✅ | Define the shared structures, union, constants, globals, and prototypes | 2026-11-16 |
| ✅ | Verify structure sizes and initialize the root directory | 2026-11-16 |
| ✅ | Implement directory-chain search and locate entries by name | 2026-11-17 |
| ✅ | Implement adding and removing directory entries and maintain `free` | 2026-11-18 |
| ✅ | Extend a directory past 25 entries using `back` and `frwd` links | 2026-11-19 |
| ✅ | Implement `CREATE U` and `CREATE D`, including duplicate-name checks | 2026-11-19 |
| ❌ | Implement `DIR`, `CD`, and `ROOT` | 2026-11-20 |
| ❌ | Test regular files, subdirectories, nested navigation, and full directories | 2026-11-21 |

### Member 2 — free sectors and file operations

| Status | Task | Due |
| --- | --- | --- |
| ✅ | Implement the initial `buildList`, `Remove`, and `Insert` operations | 2026-11-16 |
| ✅ | Add allocation/release helpers and initialize allocated sectors safely | 2026-11-18 |
| ✅ | Release complete file chains and empty directory chains for `DELETE` | 2026-11-19 |
| ✅ | Implement `OPEN` and `CLOSE`, including modes and single-open-file checks | 2026-11-20 |
| ❌ | Implement `READ` across sector boundaries | 2026-11-21 |
| ❌ | Implement `WRITE`, append behavior, and additional-sector allocation | 2026-11-22 |
| ❌ | Implement `SEEK` with position validation | 2026-11-22 |
| ❌ | Test reading, writing, appending, seeking, deletion, and sector reuse | 2026-11-23 |

### Member 3 — command interface, integration, and testing

| Status | Task | Due |
| --- | --- | --- |
| ✅ | Implement the `fgets` command loop and command identification | 2026-11-18 |
| ✅ | Parse names, modes, byte counts, and all non-`WRITE` command arguments | 2026-11-19 |
| ❌ | Parse `WRITE` while preserving spaces and validating the byte count | 2026-11-20 |
| ❌ | Validate input and add consistent, meaningful error messages | 2026-11-21 |
| ❌ | Connect every command to the directory and storage modules | 2026-11-23 |
| ❌ | Test invalid commands, duplicates, bad modes, full disk, and boundary cases | 2026-11-24 |
| ❌ | Prepare final documentation, sample input/output, and demonstration | 2026-11-25 |

## Integration checkpoints

| Status | Checkpoint | Due |
| --- | --- | --- |
| ✅ | Disk layout, root initialization, and free-sector list | 2026-11-16 |
| ❌ | Directory creation, extension, listing, and navigation | 2026-11-20 |
| ❌ | File creation, opening, closing, and deletion | 2026-11-21 |
| ❌ | Read, write, and seek across multiple sectors | 2026-11-23 |
| ❌ | Full parser connected to all operations | 2026-11-24 |
| ❌ | Final regression tests and Linux compatibility check | 2026-11-25 |

Required end-to-end cases include nested directories, files larger than 504
bytes, cross-sector reads, append and seek, deleted-sector reuse, more than 25
directory entries, duplicate names, nonempty-directory deletion, invalid modes,
disk exhaustion, and attempts to open a directory as a file.

## Git workflow

1. Pull `main`, then create a focused branch such as
   `member-1/directory-search`.
2. Work on one tracker row at a time and avoid unrelated file changes.
3. Make small commits with messages such as `feat: add directory lookup` or
   `test: cover cross-sector reads`.
4. Push the branch and open a pull request. Another member should review it.
5. Update this tracker in the same pull request when a task changes status.

Before pushing, run:

```sh
make clean
make
printf 'exit\n' | ./project
```
