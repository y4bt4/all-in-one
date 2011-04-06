#ifndef MYHASH_H
#define MEHASH_H

struct myhash {
	char ***ptr;
	int pages;
	int used;
	int count;
	int slots;
	unsigned mask;
};
struct hash_slot {
	unsigned ch;
	int alloc_len;
	int key_len;
	int data_len;
	char key[];
};

#define slot_data(s) (s->key + s->key_len)

struct myhash* make_hash(int pages,int slots);
void free_hash(struct myhash *h);
void dump_hash(struct myhash *h,int type);
void dump_hash2(struct myhash *h);

int set_hash(struct myhash *h, const char *key_ptr, int key_len, const char *data_ptr, int data_len);
void* get_hash(struct myhash *h, const char *key_ptr, int key_len, int* data_len);
struct hash_slot* get_hash_slot(struct myhash *h, const char *key_ptr, int key_len);
void del_hash(struct myhash *h, const char *key_ptr, int key_len);

//void* del_hash(struct myhash *h, const char *key_ptr, int key_len, int* data_len);
//struct hash_slot* each_hash(struct myhash *h);


unsigned calc_hash(const void *key_ptr, int key_len);

#endif
