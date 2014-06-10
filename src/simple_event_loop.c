#include "simple_event_loop.h"

#ifdef EVENT_HAVE_EPOLL
extern const event_op epoll_api;
#endif

#ifdef EVENT_HAVE_KQUEUE
extern const event_op kqueue_api;
#endif

#if 0
#ifdef EVENT_HAVE_SELECT
extern const event_op select_api;
#endif
#endif

const event_op *event_ops[] =
{
#ifdef EVENT_HAVE_EPOLL
    &epoll_api,
#endif

#ifdef EVENT_HAVE_KQUEUE
    &kqueue_api,
#endif

#if 0
#ifdef EVENT_HAVE_SELECT
    &select_api,
#endif
#endif

    NULL
};

event_loop *event_loop_create(void)
{
    event_loop *loop = (event_loop *)malloc(sizeof(event_loop));
    if(loop == NULL)
    {
        LOG("malloc");
        return NULL;
    }
    else
    {
        return loop;
    }
}

int event_loop_init(event_loop *loop)
{
    int i;
    int ret;
    const event_op *api = NULL;
    
    loop->set_size = EVENT_LOOP_INITIAL_SIZE;
    loop->max_size = EVENT_LOOP_MAX_SIZE;

    loop->registered_events = (event *)malloc(sizeof(event) * loop->set_size);
    if(NULL == loop->registered_events)
    {
        LOG("malloc");
        return -1;
    }

    loop->ready_events = (event *)malloc(sizeof(event) * loop->set_size);
    if(NULL == loop->ready_events)
    {
        LOG("malloc");
        free(loop->registered_events);
        return -1;
    }

    for(i = 0; event_ops[i] != NULL && api == NULL; i++)
    {
        api = event_ops[i];
    }
    if(api != NULL)
    {
        loop->api = api;
    }
    else
    {
        return -1;
    }

    INIT_LIST_HEAD(&loop->timeout_queue);
    loop->stop = 0;
    
    ret = loop->api->event_loop_init(loop);
    if(ret == -1)
    {
        return -1;
    }
    return 0;
}

int event_register(event_loop *loop, event *ev)
{
    int ret;
    const event_op *api = loop->api;
    event *e;

    assert(api != NULL);

    if(ev->event_fd >= loop->set_size)
    {
        ret = event_loop_resize(loop, loop->set_size * 2);
        if(ret == -1)
        {
            return -1;
        }
    }

    ret = api->event_register(loop, ev->event_fd, ev->event_type);
    if(ret == -1)
    {
        return -1;
    }
    loop->registered_events[ev->event_fd] = *ev;
    return 0;
}

int event_unregister(event_loop *loop, event *ev)
{
    int ret;
    const event_op *api = loop->api;
    
    ret = api->event_unregister(loop, ev->event_fd, ev->event_type);
    if(ret == -1)
    {
        return -1;
    }
    return 0;
    
}

int event_loop_resize(event_loop *loop, int set_size)
{
    event *registered;
    event *ready;
    int ret;
    if(set_size > loop->max_size)
    {
        errno = ERANGE;
        LOG("loop set size");
        return -1;
    }

    registered = (event *)realloc(loop->registered_events, set_size);
    if(registered == NULL)
    {
        LOG("malloc");
        return -1;
    }
    ready = (event *)realloc(loop->ready_events, set_size);
    if(ready == NULL)
    {
        LOG("realloc");
        return -1;
    }

    ret = loop->api->event_loop_resize(loop, set_size);
    if(ret == -1)
    {
        /*realloc does not need free here*/
        LOG("realloc");
        return -1;
    }

    loop->registered_events = registered;
    loop->ready_events = ready;
    loop->set_size = set_size;
}

void event_loop_main(event_loop *loop)
{
    int i;
    const event_op *api = loop->api;
    while(!loop->stop)
    {
        int events_num = api->event_loop_main(loop);
        for(i = 0; i < events_num; i++)
        {
            int fd = loop->ready_events[i].event_fd;
            event *e = &loop->registered_events[fd];
            event *r = &loop->ready_events[i];

            if(r->event_type & EVENT_ERROR)
            {
                //handle the error
            }
            else if(e->event_type & r->event_type & EVENT_READ)
            {
                e->event_cb(fd, EVENT_READ, e->event_args);
            }
            else if(e->event_type & r->event_type & EVENT_WRITE)
            {
                e->event_cb(fd, EVENT_WRITE, e->event_args);
            }
        }
    } 
}
