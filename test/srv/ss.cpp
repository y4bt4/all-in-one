#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <time.h>

int open_socket(int port) {
    int create_socket;
    struct sockaddr_in address;

    if ((create_socket = socket(AF_INET,SOCK_STREAM,0)) == -1){
	printf("The socket was not created\n");
	return 0;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(create_socket,(struct sockaddr *)&address,sizeof(address)) != 0){
	printf("Not Binding Socket\n");
	return 0;
    }
    listen(create_socket,100);
    return create_socket;
}
int accept_socket(int create_socket){
    int new_socket,addrlen;
    struct sockaddr_in address;
    addrlen = sizeof(struct sockaddr_in);
    //new_socket = accept(create_socket,(struct sockaddr *)&address,&addrlen);
    new_socket = accept(create_socket,0,(socklen_t*)&addrlen);
    if (new_socket == -1){
	printf("The Client is not connected\n");
	return 0;
    }
    return new_socket;
}
int read_and_send(int fd){
    char buf[1024];
    char buf2[2048];
    int rs;
    rs = recv(fd,buf,1023,0);
    if(rs == -1){
	return -1;
    }else if(rs == 0){
	return 0;
    }
    if(memchr(buf,10,rs)){
	time_t t = time(0);
        struct tm* stm = localtime(&t);
	buf[rs+1] = 0;
	sprintf(buf2,"%saccept %02d:%02d:%02d\n",buf,stm->tm_hour,stm->tm_min,stm->tm_sec);
	send(fd,buf2,strlen(buf2),0);
	return 0;
    }else{
	return rs;
    }
}

int loop(int listen_sock){
    #define MAX_EVENTS 10
    struct epoll_event ev, events[MAX_EVENTS];
    int conn_sock, nfds, epollfd, res, n;

    epollfd = epoll_create(10);
    if (epollfd == -1) {
	perror("epoll_create");
	return 0;
    }

    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
	perror("epoll_ctl: listen_sock");
	return 0;
    }

    for (;;) {
	nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
	if (nfds == -1) {
	    perror("epoll_pwait");
	    return 0;
	}

	for (n = 0; n < nfds; ++n) {
	    if (events[n].data.fd == listen_sock) {
		conn_sock = accept_socket(listen_sock);
		if (conn_sock == -1) {
		    perror("accept");
		    return 0;
		}
		//setnonblocking(conn_sock);
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = conn_sock;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
		    perror("epoll_ctl: conn_sock");
		    return 0;
		}
	    } else {
		conn_sock = events[n].data.fd;
		res = read_and_send(conn_sock);
		if(res == -1){
		    return 0;
		}else if(res == 0){
		    ev.events = EPOLLIN | EPOLLET;
		    ev.data.fd = conn_sock;
		    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, conn_sock, &ev) == -1) {
			perror("epoll_ctl: conn_sock");
			return 0;
		    }
		    close(conn_sock);
		}
	    }
	}
    }
}

int main(int ac,char** av){
    int port,lsn;
    if(ac < 2){
	printf("need port\n");
	return 1;
    }
    port = atoi(av[1]);
    if((lsn = open_socket(port))){
	loop(lsn);
    }
    return 0;
}

