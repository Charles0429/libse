#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "../src/se.h"

void timeout1(int fd, short type, void *args);
void timeout2(int fd, short type, void *args);

struct event_loop *loop;

int main(void)
{
    loop = event_loop_create();
    event *e1, *e2;
    int ret2 = event_loop_init(loop);
    printf("%d", ret2);
    printf("%s\n", loop->api->name);
    e1 = event_create();
    e2 = event_create();
    event_set_timeout(e1, timeout1, (void *)e1, 1000);
    event_set_timeout(e2, timeout2, (void *)e2, 500);
    event_register(loop, e1);
    event_register(loop, e2);
    event_loop_main(loop);	
}

void timeout1(int fd, short type, void *args)
{
    static i = 1;
    event *e = (event *)args;
    printf("timeout1:%d\n", i++);
    event_set_timeout(e, timeout1, (void *)e, 1000);
    event_register(loop, e);
}

void timeout2(int fd, short type, void *args)
{
    static i = 1;
    event *e = (event *)args;
    printf("timeout2:%d\n", i++);
    event_set_timeout(e, timeout2, (void *)e, 500);
    event_register(loop, e);
}

