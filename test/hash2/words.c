#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MINSIZE 6
#define MAXSIZE 12

#define LONGSIZE 10

#define LONGMINSIZE (MINSIZE * LONGSIZE)
#define LONGMAXSIZE (MAXSIZE * LONGSIZE)


char letters[] = "abcdefghjiklmnopqrstuvwxyz";
char buf[MAXSIZE + 1];
char longbuf[MAXSIZE * LONGSIZE + 1];

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
const char* longword(int* l){
	int p,i;
	*l = LONGMINSIZE + (double)random() * (LONGMAXSIZE - LONGMINSIZE) / RAND_MAX;
	for(i = 0; i < *l; i++){
		p = (double)random() * (sizeof(letters) - 1) / RAND_MAX;
		longbuf[i] = letters[p];
	}
	longbuf[i] = 0;
	return longbuf;
}
