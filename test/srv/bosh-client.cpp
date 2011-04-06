#include <iostream>
#include <string>
#include "MyTime.h"
#include <stdlib.h>
#include <stdio.h>

int request_s(const char *host,int port,const char* message,int size_message,char** response,int* size_response);


std::string request(const std::string req);
std::string login;
std::string endl("\n");
int port;
int live = 1;
int main ( int argc, char* argv[] ) {
	if(argc < 4){
		std::cerr << argv[0] << "<port> <login> <interval>" << endl;
		return 1;
	}
	login = argv[2];
	port = atoi(argv[1]);
	int timeout = atoi(argv[3]);
	std::string answer = request("login "+login);
	std::cout << answer << std::endl;

	int next = time(0) + timeout;
	int count = 0;
	char bn[64];
	while(live){
		int now = time(0);
		if(now >= next){
			sprintf(bn,"my counter %d",count);
			answer = request(bn);
			std::cout << answer << std::endl;
			next = now + timeout;
			count++;
		}
		answer = request("check");
		std::cout << answer << std::endl;
	}
	printf("Real Exit\n");
	return 0;
}


std::string request(const std::string req) {
	std::string reply;
	std::string msg = login + ": " + req + " / " + timestamp() + endl;
	char* buf;
	int size;
	int r = request_s("127.0.0.1",port,msg.data(),msg.size(),&buf,&size);
	if(r>0){
		reply.assign(buf,size);
	}else if(!r){
		printf("Exit\n");
		live = 0;
	}
	free(buf);
	return reply;
}
