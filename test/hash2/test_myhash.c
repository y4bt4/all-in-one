#include "myhash.h"
#include "words.h"
#include "hires.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


int main(int ac,char** av){
	struct myhash *hash;
	int i=0,c,r,max = 0;
	double r0,r1;
	char buf[1024];

	if(ac < 2){
		printf("%s count [dump]\n",av[0]);
		return 0;
	}
	c = atoi(av[1]);

	hash = make_hash(2,2);
	printf("m=%d used=%d pages=%d\n",max+1,hash->used,hash->pages);
	r0 = hires();
	for(i = 0; i < c; i++){
		sprintf(buf,"%010d",i);
		r = set_hash(hash,buf,strlen(buf),buf,strlen(buf));
		if(r > max){
			max = r;
		}
	}
	r1 = hires();
	printf("m=%d used=%d pages=%d tm=%f sp=%f\n",max+1,hash->used,hash->pages,r1-r0,hash->count/(r1-r0));
	
	r0 = hires();
	for(i = 0; i < c; i++){
		sprintf(buf,"%010d",i);
		r = set_hash(hash,buf,strlen(buf),buf,strlen(buf));
		if(r > max){
			max = r;
		}
	}
	r1 = hires();
	printf("m=%d used=%d pages=%d tm=%f sp=%f\n",max+1,hash->used,hash->pages,r1-r0,hash->count/(r1-r0));
	
	
	free_hash(hash);
	return 0;
}
