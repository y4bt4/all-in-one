#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(){
	int p = 10;
	int c = 0;
	while(p >>= 1){
		c++;
		printf("%d %d\n",c,p);
	}
	if(c < 2){
		c = 2;
	}
	p = 1 << (c+1);
	printf("%d %d\n",c,p);
	return 0;
}
