#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include "hires.h"


char tsbuf[128];
int mk_connect(const char *host,int port);
int send_message(int create_socket,const char* message,int size_message);
int recv_message(int create_socket, char** response,int* size_response);

const char* ts(){
	time_t t = time(0);
	struct tm* stm = localtime(&t);
	sprintf(tsbuf,"%02d:%02d:%02d",stm->tm_hour,stm->tm_min,stm->tm_sec);
	return tsbuf;
}

int mk_connect(const char *host,int port) {
	int create_socket;
	struct sockaddr_in address;
	double start,end;

	start = hires();

	if ((create_socket = socket(AF_INET,SOCK_STREAM,0)) <= 0) {
		printf("The Socket was not created\n");
		return 0;
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET,host,&address.sin_addr);

	if (connect(create_socket,(struct sockaddr *)&address,sizeof(address))){
		perror("Connect()");
		printf("The connection was not accepted with the server %s:%d...\n",inet_ntoa(address.sin_addr),port);
		return 0;
	}

	end = hires();
	if(end - start > 1.0){
		printf("Long request %s %.1f\n",ts(),end - start);
	}
	return create_socket;
}
int send_message(int create_socket,const char* message,int size_message){
	fd_set wfds;
	int retval;
	struct timeval tv;
	int rs;
	double start,end;
	start = hires();
	
	int need = size_message;

	while(need > 0){
		FD_ZERO(&wfds);
		FD_SET(create_socket, &wfds);
		tv.tv_sec = 30;
		tv.tv_usec = 0;
	
		retval = select(create_socket+1, NULL, &wfds, NULL, &tv);
		if(retval==-1){
			perror("write select()");
			close(create_socket);
			printf("write select()\n");
			return 0;
		}else if(retval){
			rs = send(create_socket,message + (size_message-need),need,0);
			need -= rs;
		}else{
			close(create_socket);
			printf("Timeout write %d %d\n",tv.tv_sec,tv.tv_usec);
			return 0;
		}
	}
	end = hires();
	if(end - start > 1.0){
	        printf("Long send %s %.1f\n",ts(),end - start);
	}
	return size_message;
}
int recv_message(int create_socket, char** response,int* size_response){
	fd_set rfds;
	int retval;
	struct timeval tv;
	int rs;
	int bufsize = 1024;
	char *buffer = (char*)malloc(bufsize);

	double start,end;
        start = hires();

	*response = buffer;
	int ready = 0;
	
	while(1){
		FD_ZERO(&rfds);
		FD_SET(create_socket, &rfds);
		tv.tv_sec = 30;
		tv.tv_usec = 0;

		retval = select(create_socket+1, &rfds, NULL, NULL, &tv);

		if (retval == -1){
			perror("read select()");
			close(create_socket);
			printf("read select()\n");
			return 0;
		}else if(retval){
			if( FD_ISSET(create_socket, &rfds)){
				rs = recv(create_socket,buffer+ready,bufsize-ready,0);

				if(rs == -1){
					perror("read()");
					close(create_socket);
					printf("read error\n");
					return 0;
				}else if(!rs){
					*size_response = ready;
					close(create_socket);
					return ready;
				}
				ready += rs;
				if(bufsize - ready < 128){
					bufsize += 1024;
					buffer = (char*)realloc(buffer,bufsize);
					*response = buffer;
				}
				if(buffer[ready-1]==10 && buffer[ready-2]==10){
        				end = hires();
				        if(end - start > 1.0){
				                printf("Long recv %s %.1f\n",ts(),end - start);
		        		}
					*size_response = ready;
					return ready;
				}
			}else{
				printf("Select return %d, but not for me\n",retval);
			}
		}else{
			printf("Timeout %d %d\n",tv.tv_sec,tv.tv_usec);
			close(create_socket);
			return -1;
		}
	}
}
