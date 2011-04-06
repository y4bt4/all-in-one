#include "myhash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


unsigned calc_hash(const void *key_ptr, int key_len) {
	unsigned h = 0x92f7e3b1;
	const unsigned char * uc_key_ptr = (const unsigned char *)key_ptr;

	while (key_len--) {
		h += *uc_key_ptr++;
		h += (h << 10);
		h ^= (h >> 6);
	}
	h += (h << 3);
	h ^= (h >> 11);
	h += (h << 15);
	return h;
}
struct myhash* make_hash(int pages,int slots){
	struct myhash* h = malloc(sizeof(struct myhash));
	int c = 0;
	while(pages >>= 1){
		c++;
	}
	if(c < 2){
		c = 2;
	}
	h->pages = 1 << (c + 1);
	init_hash(h,slots);
	return h;
}
void init_hash(struct myhash* h, int initslots){
	int i,j;
	char*** ptr = (char***)malloc(h->pages * sizeof(char**));
	for(i = 0;i < h->pages; i++){
		ptr[i] = (char**)malloc((initslots+1) * sizeof(char*));
		for(j = 0; j < initslots; j++){
			ptr[i][j] = 0;
		}
		ptr[i][j] = (char*)-1;
	}
	h->ptr = ptr;
	h->count = 0;
}
void free_hash(struct myhash *h){
	int i;
	char **ptr;
	for(i = 0; i < h->pages; i++){
		ptr = h->ptr[i];
		while(*ptr && *ptr != (char*)-1){
			free(*ptr);
			ptr++;
		}
		free(h->ptr[i]);
	}
	free(h->ptr);
}
void dump_hash(struct myhash *h,int full){
	int i;
	char **ptr;
	for(i = 0; i < h->pages; i++){
		printf("%d\n",i);
		ptr = h->ptr[i];
		while(*ptr && *ptr != (char*)-1){
			if(full){
				printf("\t%s: '%s'\n",((struct hash_slot *)*ptr)->key,slot_data(((struct hash_slot *)*ptr)));
			}else{
				printf("\t%s\n",((struct hash_slot *)*ptr)->key);
			}
			ptr++;
		}
	}
	printf("\n");
}
int set_hash(struct myhash *h, const char *key_ptr, int key_len, const char *data_ptr, int data_len){
	int p, i, size, nsize;
	char **ptr, **tmp;
	struct hash_slot *slot;
	unsigned ch = calc_hash(key_ptr,key_len);
	p = ch & (h->pages-1);
	ptr = h->ptr[p];
	while(*ptr && *ptr != (char*)-1){
		ptr++;
	}
	size = ptr - h->ptr[p];
	if(*ptr){
		nsize = 2 * (size + 1);
		h->ptr[p] = (char**)realloc(h->ptr[p],nsize * sizeof(char*));
		ptr = h->ptr[p] + size;
		for(i = size; i < nsize; i++){
			h->ptr[p][i] = 0;
		}
		h->ptr[p][i] = (char*)-1;
	}
	slot = (struct hash_slot*)(*ptr = (char*)malloc(sizeof(struct hash_slot) + key_len + data_len));
	slot->ch = ch;
	slot->key_len = key_len;
	slot->data_len = data_len;
	
	//printf("set: '%s'\n",key_ptr);
	memcpy(slot->key,key_ptr,key_len);
	//printf("res: '%s'\n",slot->key);
	memcpy(slot_data(slot),data_ptr,data_len);
	return size;
}

void* get_hash(struct myhash *h, const char *key_ptr, int key_len, int *data_len){
	int p;
	char **ptr;
	struct hash_slot *slot = 0;
	unsigned ch = calc_hash(key_ptr,key_len);
	p = ch & (h->pages-1);
	ptr = h->ptr[p];
	while(*ptr && *ptr != (char*)-1){
		slot = (struct hash_slot*)*ptr;
		if(slot->key_len == key_len && !memcmp(slot->key,key_ptr,key_len)){
			*data_len = slot->data_len;
			return slot_data(slot);
		}
		ptr++;
	}
	return 0;
}

