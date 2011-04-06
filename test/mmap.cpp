#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

void* connect(const char* fn,size_t size,int*);
void disconnect(int fd,void* ptr,size_t);
#define SIZE (1024*4*100)

int main(int,char**){
	void* ptr;
	int fd;
	if((ptr = connect("test.dat",SIZE,&fd))){
		memset(ptr,1,SIZE);
		disconnect(fd,ptr,SIZE);
	}
	return 0;
}
void* connect(const char* fn,size_t size,int* fd){
	int err;
	void * ptr;
	*fd = open(fn,O_RDWR);
	if(*fd == -1){
		err = errno;
		fprintf(stderr,"Error: %d, %s '%s'\n",err,strerror(err),fn);
		return 0;
	}
	ptr = mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,*fd,0);
	if (ptr == MAP_FAILED){
		err = errno;
		fprintf(stderr,"Error: %d, %s '%s'\n",err,strerror(err),fn);
		close(*fd);
		return 0;
	}
	return ptr;
}
void disconnect(int fd,void* ptr,size_t size){
	munmap(ptr,size);
	close(fd);
}
       //
       //void *mmap(void *addr, size_t length, int prot, int flags,
       //           int fd, off_t offset);
       //int munmap(void *addr, size_t length);
