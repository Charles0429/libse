#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "../src/se.h"

void Accept(int, short, void *arg);
void Read(int, short, void *args);
void Write(int, short, void *args);

struct event_loop *loop;

int main(void)
{
	struct event *e;
	int listenfd;
	int connfd;
	pid_t pid;
	struct sockaddr_in sa;
	struct sockaddr_in ca;
	int ca_len = sizeof(ca);
	int ret;
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		perror("socket create  error");
		exit(-1);
	}

	sa.sin_family = AF_INET;
	sa.sin_port = htons(5900);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(listenfd, (struct sockaddr *)&sa, sizeof(struct sockaddr));
	if(ret < 0)
	{
		perror("bind error");
		exit(-1);
	}

	ret = listen(listenfd, 1024);
	if(ret < 0)
	{
		perror("listen error");
		exit(-1);
	}

	loop = event_loop_create();
	int ret2 = event_loop_init(loop);
	printf("%d", ret2);
	e = event_create();
	event_set(e, listenfd, EVENT_READ, Accept, NULL);

	event_register(loop, e);
	event_loop_main(loop);	
}

void Accept(int listenfd, short event_type, void *args)
{
	struct sockaddr_in ca;
	struct event e;
	int ca_len = sizeof(ca);
	int connfd;
	
	connfd = accept(listenfd, (struct sockaddr *)&ca, &ca_len);
	if(connfd > 0)
	{
		event_set(&e, connfd, EVENT_READ, Read, NULL);
		event_register(loop, &e);	
	}
	else
	{
		perror("accept");
	}
}

void Read(int event_fd, short event_type, void *args)
{
	char buf[512];	
	struct event e;
	recv(event_fd, buf, sizeof(buf), 0);
	event_set(&e, event_fd, EVENT_WRITE, Read, NULL);
	event_unregister(loop, &e);
	event_set(&e, event_fd, EVENT_WRITE, Write, NULL);
	event_register(loop, &e); 
}

void Write(int event_fd, short event_type, void *args)
{
	struct event e;
	const static char buf[] = "HTTP/1.0 200 OK\r\n"
                                                                                                                "Content-type:text/html\r\n"
                                                                                                                "Content-length:16\r\n\r\n"
                                                                                                                "web server test!";
        send(event_fd, buf, sizeof(buf) - 1, 0);
	event_set(&e, event_fd, event_type, Write, NULL);
	event_unregister(loop, &e);
        close(event_fd);
	//e.event_fd = event_fd;
	//event_unregister(loop, &e);
}
