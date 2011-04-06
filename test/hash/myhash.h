#ifndef MYHASH_H
#define MEHASH_H

struct myhash {
	char ***ptr;
	int pages;
	int count;
};
struct hash_slot {
	unsigned ch;
	int key_len;
	int data_len;
	char key[];
};

#define slot_data(s) (s->key + s->key_len)

struct myhash* make_hash(int pages,int slots);
void init_hash(struct myhash* h,int inislots);
void free_hash(struct myhash *h);
void dump_hash(struct myhash *h,int flag);

int set_hash(struct myhash *h, const char *key_ptr, int key_len, const char *data_ptr, int data_len);
void* get_hash(struct myhash *h, const char *key_ptr, int key_len, int* data_len);


unsigned calc_hash(const void *key_ptr, int key_len);

#endif