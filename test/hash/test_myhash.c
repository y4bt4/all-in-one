#include "myhash.h"
#include "words.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


int main(int ac,char** av) {
	int i, j, s, count, pages, seed, cc1 = 0, cc2 = 0, c1, c2, m1 = 0, m2 = 0, f1 = 0, f2 = 0;
	struct myhash *hash;
	const char *w;
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

	hash = make_hash(pages,2);

	for(i = 0; i < count; i++){
		w = word(&s);
		printf("w: '%s'\n",w);
		if(c1 = set_hash(hash,w,s+1,w,s+1)){
			if(c1 > m1){
				m1 = c1;
			}
			cc1++;
		}else{
			f1++;
		}
	}
	printf("\ntry=%d m=%d c=%d f=%d\n",count,m1,cc1,f1);

	dump_hash(hash,1);
	free_hash(hash);

	return 0;
}
