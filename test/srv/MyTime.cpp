#include "MyTime.h"
#include <stdio.h>

std::string timestamp(time_t t){
	if(!t){
		t = time(0);
	}
	struct tm* stm = localtime(&t);
	char buf[1024];
	sprintf(buf,"%02d:%02d:%02d",stm->tm_hour,stm->tm_min,stm->tm_sec);
	return std::string(buf);
}
