#include "filesystem.h"

#include <stdlib.h>
#include <string.h>

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
