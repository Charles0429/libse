#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "../src/se.h"

void Accept(int, short, void *arg);

int main(void)
{
	struct event_loop *loop;
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

	ret = listen(listenfd, 5);
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
	int ca_len = sizeof(ca);
	int connfd;
	
	connfd = accept(listenfd, (struct sockaddr *)&ca, &ca_len);
	if(connfd)
	{
		printf("a new connection %d\n", connfd);
		return;
	} 
}
