#include "simple_event.h"

event *event_create(void)
{
    struct event *ev;

    ev = (event *)malloc(sizeof(event));
    return ev;
}

void event_set(struct event *ev, int ev_fd, 
               short ev_type, event_callback ev_cb, 
               void *ev_args)
{
    ev->event_fd = ev_fd;
    ev->event_cb = ev_cb;
    ev->event_type = ev_type;
    ev->event_args = ev_args;
}

void event_destory(struct event *ev)
{
    free(ev);
}
