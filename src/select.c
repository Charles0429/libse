#ifndef _SELECT_H
#define _SELECT_H
#include <sys/select.h>
#include "simple_event_loop.h"

typedef struct select_api_data
{
    fd_set read_sets;
    fd_set write_sets;
    fd_set t_read_sets;
    fd_set t_write_sets;
    int max_read_fd;
    int max_write_fd;
}select_api_data;

int select_init(event_loop *);
int select_register(event_loop *, int, short);
int select_unregister(event_loop *, int, short);
int select_resize(event_loop *, int);
int select_main(event_loop *);

const struct event_op select_api = 
{
    "select",
    &select_init,
    &select_register,
    &select_unregister,
    &select_resize,
    &select_main
};

int select_init(event_loop *loop)
{
    select_api_data *api_data;
    assert(loop != NULL);

    api_data = (select_api_data *)malloc(sizeof(select_api_data));
    if(api_data == NULL)
    {
        event_log_debug3(loop->log, EMERG, "select_init:%s, file:%s, line:%d\n", strerror(errno), __FILE__, __LINE__);
        return -1;
    }

    FD_ZERO(&api_data->read_sets);
    FD_ZERO(&api_data->t_read_sets);
    FD_ZERO(&api_data->write_sets);
    FD_ZERO(&api_data->t_write_sets);

    api_data->max_read_fd = -1;
    api_data->max_write_fd = -1;

    loop->api_data = api_data;
    return 0;
}

int select_register(event_loop *loop, int fd, short type)
{
    select_api_data *api_data = loop->api_data;

    if(type & EVENT_READ)
    {
        int max_read_fd = api_data->max_read_fd;
        FD_SET(fd, &api_data->t_read_sets);
        api_data->max_read_fd = (max_read_fd > fd ? max_read_fd : fd);
    }
    if(type & EVENT_WRITE)
    {
        int max_write_fd = api_data->max_write_fd;
        FD_SET(fd, &api_data->t_write_sets);
        api_data->max_write_fd = (max_write_fd > fd ? max_write_fd : fd);
    }

    return 0;
}

int select_unregister(event_loop *loop, int fd, short type)
{
    select_api_data *api_data = loop->api_data;
    int i;

    if(type & EVENT_READ)
    {
        int max_read_fd = api_data->max_read_fd;
        FD_CLR(fd, &api_data->t_read_sets);
        for(i = max_read_fd - 1; i >= 0; i--)
        {
            if(FD_ISSET(i, &api_data->t_read_sets))
            {
                api_data->max_read_fd = i;
                break;
            }
        }
        if(api_data->max_read_fd == max_read_fd)
        {
            api_data->max_read_fd = -1;
        }
    }
    if(type & EVENT_WRITE)
    {
        int max_write_fd = api_data->max_write_fd;
        FD_CLR(fd, &api_data->t_write_sets);
        for(i = max_write_fd - 1; i >= 0; i--)
        {
            if(FD_ISSET(i, &api_data->t_write_sets))
            {
                api_data->max_write_fd = i;
                break;
            }
        }
        if(api_data->max_write_fd == max_write_fd)
        {
            api_data->max_write_fd = -1;
        }
    }
    return 0;
}

int select_resize(event_loop *loop, int set_size)
{
    if(set_size > FD_SETSIZE)
    {
        return -1;
    }
    return 0;
}

int select_main(event_loop *loop)
{
    select_api_data *api_data = loop->api_data;
    int ret;
    int events_num;
    int nfds;
    int i;

    nfds = api_data->max_read_fd > api_data->max_write_fd ? 
               api_data->max_read_fd : api_data->max_write_fd;
    nfds += 1;

    api_data->read_sets = api_data->t_read_sets;
    api_data->write_sets = api_data->t_write_sets;

    ret = select(nfds, &api_data->read_sets, &api_data->write_sets, 
                 NULL, NULL);
    
    events_num = 0;
    for(i = 0; i < nfds; i++)
    {
        int event_type = 0;
        if(FD_ISSET(i, &api_data->read_sets))
        {
            event_type |= EVENT_READ;
        }
        if(FD_ISSET(i, &api_data->write_sets))
        {
            event_type |= EVENT_WRITE;
        }

        if(event_type)
        {
            loop->ready_events[events_num].event_fd = i;
            loop->ready_events[events_num].event_type = event_type;
            events_num++;
        }
    }

    return events_num; 
}

#endif
