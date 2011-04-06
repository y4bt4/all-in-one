#include "myhash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ALSIZE 5

unsigned calc_hash1(const void *key_ptr, int key_len) {
	unsigned h = 0x92f7e3b1;
	unsigned char * uc_key_ptr = (unsigned char *)key_ptr;

	while (key_len--) {
		h = (h << 4) + (h >> 28) + *uc_key_ptr++;
	}
	return h;
}
unsigned calc_hash(const void *key_ptr, int key_len) {
	unsigned h = 0x92f7e3b1;
	//unsigned h = 0;
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
char*** _make_ptr(int pages, int initslots);
char*** _make_ptr(int pages, int initslots){
	int i,j;
	char*** ptr = (char***)malloc(pages * sizeof(char**));
	for(i = 0;i < pages; i++){
		ptr[i] = (char**)malloc((initslots+1) * sizeof(char*));
		for(j = 0; j < initslots; j++){
			ptr[i][j] = 0;
		}
		ptr[i][j] = (char*)-1;
	}
	return ptr;
}
struct myhash* make_hash(int pages,int slots){
	struct myhash* h = (struct myhash*)malloc(sizeof(struct myhash));
	int c = 0;
	while(pages >>= 1){
		c++;
	}
	if(c < 2){
		c = 2;
	}
	h->pages = 1 << (c + 1);
	h->mask = h->pages - 1;
	if(slots < 3){
		slots = 3;
	}
	h->slots = slots;
	h->ptr = _make_ptr(h->pages,h->slots);
	h->used = 0;
	h->count = 0;
	return h;
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
void dump_hash(struct myhash *h,int type){
	int i;
	char **ptr;
	printf("count=%d used=%d pages=%d\n",h->count,h->used,h->pages);
	if(type==3){
		return;
	}
	for(i = 0; i < h->pages; i++){
		ptr = h->ptr[i];
		while(*ptr && *ptr != (char*)-1){
			ptr++;
		}
		if(ptr - h->ptr[i] == 0 && !*ptr){
			continue;
		}
		printf("%d: %d\n",i,ptr - h->ptr[i]);
		ptr = h->ptr[i];
		if(type==0){
			continue;
		}
		while(*ptr && *ptr != (char*)-1){
			if(type==2){
				printf("\t%s: '%s'\n",((struct hash_slot *)*ptr)->key,slot_data(((struct hash_slot *)*ptr)));
			}else if(type==1){
				printf("%08x %s\n",
					((struct hash_slot *)*ptr)->ch,
					//((struct hash_slot *)*ptr)->ch & (h->pages - 1),
					((struct hash_slot *)*ptr)->key);
			}
			ptr++;
		}
	}
	printf("\n");
}
void dump_hash2(struct myhash *h){
	int i;
	char **ptr;
	printf("count=%d used=%d pages=%d\n",h->count,h->used,h->pages);
	for(i = 0; i < h->pages; i++){
		printf("%d\n",i);
		ptr = h->ptr[i];
		while(*ptr != (char*)-1){
			if(*ptr){
				printf("\t%s\n",((struct hash_slot *)*ptr)->key);
			}else{
				printf("\t....\n");
			}
			ptr++;
		}
	}
	printf("\n");
}
int set_hash(struct myhash *h, const char *key_ptr, int key_len, const char *data_ptr, int data_len){
	int p, i, j, size, nsize, needsize, npages,used;
	char **ptr, **tmp;
	char*** new_ptr;
	struct hash_slot *slot;
	
	if(h->mask <= h->count){
		new_ptr = _make_ptr(h->pages*2,h->slots);
		npages = h->pages * 2 - 1;
		used = 0;
		for(i = 0; i < h->pages; i++){
			ptr = h->ptr[i];
			while(*ptr && *ptr != (char*)-1){
				slot = (struct hash_slot*)*ptr;
				p = slot->ch & npages;
				tmp = new_ptr[p];
				while(*tmp && *tmp != (char*)-1){
					++tmp;
				}
				size = tmp - new_ptr[p];
				if(*tmp){
					nsize = 2 * (size + 1);
					new_ptr[p] = (char**)realloc(new_ptr[p],nsize * sizeof(char*));
					tmp = new_ptr[p] + size;
					for(j = size+1; j < nsize; j++){
						new_ptr[p][j] = 0;
					}
					new_ptr[p][j] = (char*)-1;
				}
				if(size == 0){
					++used;
				}
				*tmp = *ptr;
				++ptr;
			}
			free(h->ptr[i]);
		}
		free(h->ptr);
		h->pages = ( h->mask = npages ) + 1;
		h->used = used;
		h->ptr = new_ptr;
	}

	unsigned ch = calc_hash(key_ptr,key_len);
	needsize = (((sizeof(struct hash_slot) + key_len + data_len) >> ALSIZE) + 1) << ALSIZE;
	p = ch & h->mask;
	ptr = h->ptr[p];
	while(*ptr && *ptr != (char*)-1){
		slot = (struct hash_slot*)*ptr;
		if(slot->key_len == key_len && !memcmp(slot->key,key_ptr,key_len)){
			if(slot->alloc_len >= needsize){
				slot->data_len = data_len;
				memcpy(slot_data(slot),data_ptr,data_len);
			}else{
				slot = (struct hash_slot*)(*ptr = (char*)realloc(slot,needsize));
				slot->alloc_len = needsize;
				slot->data_len = data_len;
				memcpy(slot_data(slot),data_ptr,data_len);
			}
			return ptr - h->ptr[p];
		}
		ptr++;
	}
	size = ptr - h->ptr[p];

	if(*ptr){
		nsize = 2 * (size + 1);
		h->ptr[p] = (char**)realloc(h->ptr[p],nsize * sizeof(char*));
		ptr = h->ptr[p] + size;
		for(i = size+1; i < nsize; i++){
			h->ptr[p][i] = 0;
		}
		h->ptr[p][i] = (char*)-1;
	}

	slot = (struct hash_slot*)(*ptr = (char*)malloc(needsize));
	slot->alloc_len = needsize;
	slot->ch = ch;
	slot->key_len = key_len;
	slot->data_len = data_len;
	
	memcpy(slot->key,key_ptr,key_len);
	memcpy(slot_data(slot),data_ptr,data_len);
	if(size==0){
		++h->used;
	}
	++h->count;
	return size;
}

void* get_hash(struct myhash *h, const char *key_ptr, int key_len, int *data_len){
	int p;
	char **ptr;
	struct hash_slot *slot = 0;
	unsigned ch = calc_hash(key_ptr,key_len);
	p = ch & h->mask;
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
struct hash_slot* get_hash_slot(struct myhash *h, const char *key_ptr, int key_len){
	int p;
	char **ptr;
	struct hash_slot *slot = 0;
	unsigned ch = calc_hash(key_ptr,key_len);
	p = ch & h->mask;
	ptr = h->ptr[p];
	while(*ptr && *ptr != (char*)-1){
		slot = (struct hash_slot*)*ptr;
		if(slot->key_len == key_len && !memcmp(slot->key,key_ptr,key_len)){
			return slot;
		}
		ptr++;
	}
	return 0;
}


void del_hash(struct myhash *h, const char *key_ptr, int key_len){
	int p,i,f;
	char **ptr;
	struct hash_slot *slot = 0;
	unsigned ch = calc_hash(key_ptr,key_len);
	p = ch & h->mask;
	ptr = h->ptr[p];
	while(*ptr && *ptr != (char*)-1){
		slot = (struct hash_slot*)*ptr;
		if(slot->key_len == key_len && !memcmp(slot->key,key_ptr,key_len)){
			break;
		}else{
			slot = 0;
		}
		ptr++;
	}
	if(slot){
		f = ptr - h->ptr[p];
		free(*ptr);
		*ptr = 0;

		ptr++;
		while(*ptr && *ptr != (char*)-1){
			ptr++;
		}
		--ptr;
		if(*ptr){
			h->ptr[p][f] = *ptr;
			*ptr = 0;
		}else if(f==0){
			--h->used;
		}
		--h->count;
	}
}


