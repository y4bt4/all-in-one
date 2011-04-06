#include "myhash.h"
#include "words.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

const char *dk = "hgmiboz";
const char *dk2 = "xxdthcxp";
const char *list[] = {
 "00","01","02","03","04","05","06","07","08","09"
,"10","11","12","13","14","15","16","17","18","19"
,"20","21","22","23","24","25","26","27","28","29"
,"30","31","32","33","34","35","36","37","38","39"
,"40","41","42","43","44","45","46","47","48","49"
};

int main(int ac,char** av){
	struct myhash *hash;
	int i,c,r,max,d = 0;
	char buf[1024];

	if(ac < 2){
		printf("%s count [dump]\n",av[0]);
		return 0;
	}
	c = atoi(av[1]);
	if(ac == 3){
		d = 1;
	}
	hash = make_hash(7,4);
	for(i = 0; i < c; i++){
		sprintf(buf,"%04d",i);
		r = set_hash(hash,buf,strlen(buf)+1,buf,strlen(buf)+1);
		if(r > max){
			max = r;
		}
		printf("i=%d m=%d '%s'\n",i,max+1,buf);
		dump_hash(hash,3);
	}
	free_hash(hash);
	return 0;
}
int main3(){
	struct myhash *hash;
	int i,c,r,max;
	hash = make_hash(7,4);
	c = sizeof(list) / sizeof(*list);
	for(i = 0; i < c; i++){
		r = set_hash(hash,list[i],strlen(list[i])+1,list[i],strlen(list[i])+1);
		if(r > max){
			max = r;
		}
		printf("i=%d m=%d '%s'\n",i,max+1,list[i]);
		dump_hash(hash,0);
	}
	free_hash(hash);
	return 0;
}


int main2(int ac,char** av) {
	int dump, i, j, s, ls, count, pages, seed, cc1 = 0, c1, m1 = 0, f1 = 0;
	struct myhash *hash;
	const char *w,*lw;
	if(ac < 4){
		printf("%s seed pages count [dump]\n",av[0]);
		return 0;
	}
	if(ac == 5){
		dump = atoi(av[4]);
	}else{
		dump = 0;
	}

	seed = atoi(av[1]);
	if(!seed){
		seed = time(0);
	}
	
	pages = atoi(av[2]);
	count = atoi(av[3]);

	hash = make_hash(pages,3);

	srand(seed);
	//printf("\ntry=%d m=%d c=%d f=%d u=%d p=%d\n",count,m1,cc1,f1,hash->used,hash->pages);
	for(i = 0; i < count; i++){
		printf("\ntry=%d m=%d c=%d f=%d u=%d p=%d\n",count,m1,cc1,f1,hash->used,hash->pages);
		w = word(&s);
		lw = longword(&ls);
		printf("w: '%s'\n",w);
		if(c1 = set_hash(hash,w,s+1,lw,ls+1)){
			if(c1 > m1){
				m1 = c1;
			}
			cc1++;
		}else{
			f1++;
		}
	}
	printf("\ntry=%d m=%d c=%d f=%d u=%d p=%d\n",count,m1,cc1,f1,hash->used,hash->pages);
	dump_hash2(hash);

	//printf("deleting: '%s'\n",dk);
	//del_hash(hash,dk,strlen(dk)+1);

	//printf("deleting: '%s'\n",dk2);
	//del_hash(hash,dk2,strlen(dk2)+1);

	//dump_hash2(hash);

	//srand(seed);
	//cc1 = 0;
	//m1 = 0;
	//f1 = 0;
	//for(i = 0; i < count; i++){
	//	w = word(&s);
	//	lw = longword(&ls);
	//	printf("w: '%s'\n",w);
	//	if(c1 = set_hash(hash,w,s+1,lw,ls+1)){
	//		if(c1 > m1){
	//			m1 = c1;
	//		}
	//		cc1++;
	//	}else{
	//		f1++;
	//	}
	//}
	//printf("\ntry=%d m=%d c=%d f=%d u=%d p=%d\n",count,m1,cc1,f1,hash->used,hash->pages);

	free_hash(hash);

	return 0;
}
