#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MINSIZE 6
#define MAXSIZE 12

char letters[] = "abcdefghjiklmnopqrstuvwxyz";
char buf[MAXSIZE+1];

const char* word(int* l){
	int p,i;
	*l = MINSIZE + (double)random() * (MAXSIZE - MINSIZE) / RAND_MAX;
	for(i = 0; i < *l; i++){
		p = (double)random() * (sizeof(letters) - 1) / RAND_MAX;
		buf[i] = letters[p];
	}
	buf[i] = 0;
	return buf;
}
