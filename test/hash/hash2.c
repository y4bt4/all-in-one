#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MINSIZE 6
#define MAXSIZE 12


char letters[] = "abcdefghjiklmnopqrstuvwxyz";
char buf[MAXSIZE+1];

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
	unsigned  h = 0x92f7e3b1;
	unsigned char * uc_key_ptr = (unsigned char *)key_ptr;
	//unsigned char * uc_key_ptr_end = uc_key_ptr + key_len;
	
	while (key_len--) {
		h = (h << 4) + (h >> 28) + *uc_key_ptr++;
	}
	return h;
}
unsigned hash2(void *key_ptr, int key_len) {
	unsigned  h = 0x92f7e3b1;
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
void calc(int pages,int slots,unsigned h,int *page, int *slot){
	*page = h % pages;
	*slot = (h / pages) % slots;
}
int compare(char*** has,unsigned h,int pages,int slots){
	int page,slot,c = 0;
	calc(pages,slots,h,&page,&slot);
	if(has[page][slot]){
		c++;
	}else{
		has[page][slot] = (char*)malloc(MAXSIZE+1);
		strcpy(has[page][slot],buf);
	}
	return c;
}
void store(char*** has, int page, int slots){
	int i;
	char **p = has[page];
	for(i = 0; i < slots; i++){
		if(!p[i]){
			p[i] = (char*)malloc(MAXSIZE+1);
			strcpy(p[i],buf);
			break;
		}
	}
}
int main(int ac,char** av) {
	int i, j, s, count, pages, slots, seed, cc1 = 0, cc2 = 0;
	char ***has1,***has2;
	if(ac < 5){
		printf("%s seed pages slots count\n",av[0]);
		return 0;
	}

	seed = atoi(av[1]);
	if(seed){
		srand(seed);
	}else{
		srand(time(0));
	}
	pages = atoi(av[2]);
	slots = atoi(av[3]);
	count = atoi(av[4]);
	has1 = (char***)malloc(pages * sizeof(char**));
	has2 = (char***)malloc(pages * sizeof(char**));
	for(i = 0; i < pages; i++){
		has1[i] = (char**)malloc(slots * sizeof(char*));
		has2[i] = (char**)malloc(slots * sizeof(char*));
		for(j = 0; j < slots; j++){
			has1[i][j] = 0;
			has2[i][j] = 0;
		}
	}

	for(i = 0; i < count; i++){
		word(&s);
		cc1 += compare(has1,hash1(buf,s),pages,slots);
		cc2 += compare(has2,hash2(buf,s),pages,slots);
	}

	for(i = 0; i < pages; i++){
		for(j = 0; j < slots; j++){
			if(has1[i][j]){
				free(has1[i][j]);
			}
			if(has2[i][j]){
				free(has2[i][j]);
			}
		}
		free(has1[i]);
		free(has2[i]);
	}
	free(has1);
	free(has2);
	printf("\ncap=%d try=%d c1=%d c2=%d\n",pages*slots,count,cc1,cc2);
	return 0;
}
