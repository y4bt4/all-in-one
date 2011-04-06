/* client.c */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc,char *argv[]) {
	int create_socket;
	int bufsize = 1024;
	char *buffer = (char*)malloc(bufsize);
	struct sockaddr_in address;
	
	printf("\x1B[2J");
	if ((create_socket = socket(AF_INET,SOCK_STREAM,0)) > 0)
		printf("The Socket was created\n");
	address.sin_family = AF_INET;
	address.sin_port = htons(15000);
	inet_pton(AF_INET,argv[1],&address.sin_addr);
	
	if (connect(create_socket,(struct sockaddr *)&address,sizeof(address)) == 0)
		printf("The connection was accepted with the server %s...\n",inet_ntoa(address.sin_addr));
	do{
		recv(create_socket,buffer,bufsize,0);
		printf("Message recieved: %s\n",buffer);
		if (strcmp(buffer,"/q\n")){
			printf("Message to send: ");
			fgets(buffer,bufsize,stdin);
			send(create_socket,buffer,bufsize,0);
		}
	}while (strcmp(buffer,"/q\n"));
	close(create_socket);
}
