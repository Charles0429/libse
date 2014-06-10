#include <sys/epoll.h>
#include "log.h"
#include "simple_event_loop.h"

int epoll_init(event_loop *loop);
int epoll_register(event_loop *loop, int fd,  short type);
int epoll_unregister(event_loop *loop, int fd, short type);
int epoll_resize(event_loop *loop, int set_size);
int epoll_main(event_loop *loop);

event_op epoll_api = 
{
    &epoll_init,
    &epoll_register,
    &epoll_unregister,
    &epoll_resize,
    &epoll_main
};

typedef struct epoll_api_data
{
    int epfd;
    struct epoll_event *events;
}epoll_api_data;

int epoll_init(event_loop *loop)
{
    epoll_api_data *api_data;

    api_data = (epoll_api_data *)malloc(sizeof(epoll_api_data));
    if(api_data == NULL)
    {
        LOG("malloc"); 
        return -1;
    }

    api_data->events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * loop->set_size);
    if(api_data == NULL)
    {
        LOG("malloc");
        free(api_data);
        return -1;
    }

    api_data->epfd = epoll_create(1024);
    if(api_data->epfd == -1)
    {
        LOG("epoll_create");
        free(api_data->events);
        free(api_data);
        return -1;
    }

    loop->api_data = api_data;
    return 0;
}

int epoll_register(event_loop *loop, int fd,  short type)
{
    epoll_api_data *api_data = loop->api_data;
    struct epoll_event event;
    int ret = 0;
    
    event.data.fd = fd;
    event.events = 0;
    if(type & EVENT_READ)
    {
        event.events |= EPOLLIN;
    }
    if(type & EVENT_WRITE)
    {
        event.events |= EPOLLOUT;
    }
    
    ret = epoll_ctl(api_data->epfd, EPOLL_CTL_ADD, fd, &event);
    if(ret == -1)
    {
        LOG("epoll_ctl");
        return -1;
    }

    return 0;
}

int epoll_unregister(event_loop *loop, int fd, short type)
{
    epoll_api_data *api_data = loop->api_data;
    struct epoll_event event;
    int ret;

    event.events = 0;
    event.data.fd = fd;
    if(type & EVENT_READ)
    {
        event.events |= EPOLLIN;
    }
    if(type & EVENT_WRITE)
    {
        event.events |= EPOLLOUT;
    }

    ret = epoll_ctl(api_data->epfd, EPOLL_CTL_DEL, fd, &event);
    if(ret == -1)
    {
        LOG("epoll_ctl");
        return -1;
    }
    return 0;
}

int epoll_resize(event_loop *loop, int set_size)
{
    epoll_api_data *api_data = loop->api_data;
    struct epoll_event *events_ptr;

    events_ptr = realloc(api_data->events, sizeof(struct epoll_event) * set_size);
    if(events_ptr != NULL)
    {
        api_data->events = events_ptr;
        return 0;
    }
    else
    {
        return -1;
    }
}

int epoll_main(event_loop *loop)
{
    epoll_api_data *api_data = loop->api_data;
    int ret;
    int i;
    int events_num;

    ret = epoll_wait(api_data->epfd, api_data->events,
                     loop->set_size, -1);

    events_num = ret;
    for(i = 0; i < events_num; i++)
    {
        int event_type = 0;
        struct epoll_event *e = &api_data->events[i];

        if(e->events & EPOLLERR)
        {
            event_type |= EVENT_ERROR;
        }
        else if(e->events & (EPOLLIN | EPOLLHUP))
        {
            event_type |= EVENT_READ;
        }
        else if(e->events & EPOLLOUT)
        {
            event_type |= EVENT_WRITE;
        }

        loop->ready_events[i].event_fd = e->data.fd;
        loop->ready_events[i].event_type = event_type;
    }
    return events_num;
}

