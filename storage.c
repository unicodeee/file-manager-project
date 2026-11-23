#include "filesystem.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

int allocateSector(void){
	int sector_number = Remove();

	if(sector_number == -1){
		return -1;
	}

	memset(&sector[sector_number],0,sizeof(sector[sector_number]));

	sector[sector_number].data.backf = -1;
	sector[sector_number].data.frwdf = -1;

	return sector_number;
}

bool releaseSector(int sector_number){
	if(sector_number <= 0 || sector_number >= NUM_SECTORS){
		return false;
	}

	if(!Insert(sector_number)){
		return false;
	}

	memset(&sector[sector_number],0,sizeof(sector[sector_number]));

	sector[sector_number].data.backf = -1;
	sector[sector_number].data.frwdf = -1;

	return true;
}

bool releaseFileChain(int first_sector){
	int chain[NUM_SECTORS];
	bool visited[NUM_SECTORS] = {false};
	int count = 0;
	int current = first_sector;

	while(current != -1){
		if(current <= 0 || current >= NUM_SECTORS || visited[current] || count >= NUM_SECTORS){
			return false;
		}
		visited[current] = true;
		chain[count++] = current;

		current = sector[current].data.frwdf;
	}
	for(int i=0; i < count; i++){
		if(!releaseSector(chain[i])){
			return false;
		}
	}
	return true;
}

bool releaseDirectoryChain(int first_sector){
	int chain[NUM_SECTORS];
	bool visited[NUM_SECTORS] = {false};
	int count = 0;
	int current = first_sector;

	while(current != 0 && current != -1){
		if(current < 0 || current >= NUM_SECTORS || visited[current] || count >= NUM_SECTORS){
			return false;	
		}
		visited[current] = true;
		
		for(int i = 0; i < DIRECTORY_ENTRIES; i++){
			if(sector[current].directory.dir[i].type != TYPE_FREE){
				return false;
			}
		}

		chain[count++] = current;
		current = sector[current].directory.frwd;
	}

	for(int i = 0; i < count; i++){
		if(!releaseSector(chain[i])){
			return false;
		}
	}
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
    if(open_file.is_open){
	puts("Error: A file is already open.");
	return false;
    }
    if(name == NULL || name[0] == '\0' || strlen(name) > MAX_NAME_LENGTH){
	puts("Error: Invalid file name.");
	return false;
    }

    mode = (char)toupper((unsigned char)mode);

    if(mode != 'R' && mode != 'W'){
	puts("Error: Mode must be R (read) or W (write).");
	return false;
    }

    int current = current_directory;
    bool visited[NUM_SECTORS] = {false};

    while(current >= 0 && current < NUM_SECTORS){
	if(visited[current]){
		puts("Error: Invalid directory chain.");
		return false;
	}

	visited[current] = true;

	for(int i=0; i < DIRECTORY_ENTRIES; i++){
		Directory *entry = &sector[current].directory.dir[i];

		if(entry->type != TYPE_USER_FILE){
			continue;	
		}
		
		if(strncmp(entry->name, name, MAX_NAME_LENGTH + 1) == 0){
			open_file.is_open = true;
			open_file.directory_sector = current;
			open_file.entry_index = i;
			open_file.mode = mode;
			open_file.position = 0;

			printf("Opened '%s' in '%s' mode.\n",
				name, mode == 'R' ? "read" : "write");
			return true;
		}
	}
	int next = sector[current].directory.frwd;

	if(next == 0 || next >= NUM_SECTORS){
		puts("Error: Invalid directory chain.");
		return false;
	}

	current = next;
    }
    puts("Error: File not found.");
    return false;
}

void closeFile(void)
{
    if(!open_file.is_open){
	puts("Error: No file is currently open.");
	return;
    }
    open_file = (OpenFileState){false, -1, -1, '\0', 0};
    
    puts("File closed.");
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
