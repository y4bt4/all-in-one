#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MINSIZE 6
#define MAXSIZE 12

char letters[] = "abcdefghjiklmnopqrstuvwxyz";
char buf[MAXSIZE+1];
typedef unsigned (*hnd)(void*,int);

void word(int* l){
	int p,i;
	*l = MINSIZE + (double)random() * (MAXSIZE - MINSIZE) / RAND_MAX;
	for(i = 0; i < *l; i++){
		p = (double)random() * (sizeof(letters) - 1) / RAND_MAX;
		buf[i] = letters[p];
	}
	buf[i] = 0;
}

unsigned hash1(void *key_ptr, int key_len) {
	unsigned h = 0x92f7e3b1;
	unsigned char * uc_key_ptr = (unsigned char *)key_ptr;
	//unsigned char * uc_key_ptr_end = uc_key_ptr + key_len;

	while (key_len--) {
		h = (h << 4) + (h >> 28) + *uc_key_ptr++;
	}
	return h;
}
unsigned hash2(void *key_ptr, int key_len) {
	unsigned h = 0x92f7e3b1;
	unsigned char * uc_key_ptr = (unsigned char *)key_ptr;
	//unsigned char * uc_key_ptr_end = uc_key_ptr + key_len;

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
char*** init(int pages, int initslot){
	int i,j;
	char*** ptr = (char***)malloc(pages * sizeof(char**));
	for(i = 0;i < pages; i++){
		ptr[i] = (char**)malloc((initslot+1) * sizeof(char*));
		for(j = 0; j < initslot; j++){
			ptr[i][j] = 0;
		}
		ptr[i][j] = (char*)-1;
	}
	return ptr;
}
void destroy(char*** has, int pages){
	int i;
	char **ptr;
	for(i = 0; i < pages; i++){
		ptr = has[i];
		while(*ptr && *ptr != (char*)-1){
			free(*ptr);
			ptr++;
		}
		free(has[i]);
	}
	free(has);
}
void dump(char*** has, int pages){
	int i;
	char **ptr;
	for(i = 0; i < pages; i++){
		printf("%d\n",i);
		ptr = has[i];
		while(*ptr && *ptr != (char*)-1){
			printf("\t%s\n",*ptr);
			ptr++;
		}
	}
	printf("\n");
}
int store(char*** has, int pages, char* key, int len, hnd fun){
	int p, i, size, nsize;
	char **ptr, **tmp;
	unsigned h = (*fun)(key,len);
	p = h % pages;
	ptr = has[p];
	//printf("store: '%s'\n", key);
	while(*ptr && *ptr != (char*)-1){
		ptr++;
	}
	size = ptr - has[p];
	if(*ptr){
		nsize = 2 * (size + 1);
		has[p] = (char**)realloc(has[p],nsize * sizeof(char*));
		ptr = has[p] + size;
		for(i = size; i < nsize; i++){
			has[p][i] = 0;
		}
		has[p][i] = (char*)-1;
	}
	*ptr = (char*)malloc(MAXSIZE + 1);
	strcpy(*ptr,key);
	return size;
}

int main(int ac,char** av) {
	int i, j, s, count, pages, seed, cc1 = 0, cc2 = 0, c1, c2, m1 = 0, m2 = 0, f1 = 0, f2 = 0;
	char ***has1,***has2;
	if(ac < 4){
		printf("%s seed pages count\n",av[0]);
		return 0;
	}

	seed = atoi(av[1]);
	if(seed){
		srand(seed);
	}else{
		srand(time(0));
	}
	pages = atoi(av[2]);
	count = atoi(av[3]);

	has1 = init(pages,1);
	has2 = init(pages,1);

	for(i = 0; i < count; i++){
		word(&s);
		if(c1 = store(has1,pages,buf,s,&hash1)){
			if(c1 > m1){
				m1 = c1;
			}
			cc1++;
		}else{
			f1++;
		}
		if(c2 = store(has2,pages,buf,s,&hash2)){
			if(c2 > m2){
				m2 = c2;
			}
			cc2++;
		}else{
			f2++;
		}
		//printf("%2d %2d %s\n",c1,c2,buf);
	}
	printf("\ntry=%d m1=%d m2=%d c1=%d c2=%d f1=%d f2=%d\n",count,m1,m2,cc1,cc2,f1,f2);
	//dump(has1,pages);
	//dump(has2,pages);

	destroy(has1,pages);
	destroy(has2,pages);

	return 0;
}
