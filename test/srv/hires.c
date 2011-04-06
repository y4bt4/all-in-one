#include "hires.h"
#include <sys/time.h>
#include <stdlib.h>

double hires(){
	struct timeval tv;
	if(gettimeofday(&tv,0)){
		exit(1);
	}
	//msec = tv.tv_usec;
	return ((double)tv.tv_sec) + ((double)tv.tv_usec)/1000000.0;
}
