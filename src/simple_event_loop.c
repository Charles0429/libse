#include "simple_event_loop.h"

#ifdef EVENT_HAVE_EPOLL
extern const event_op epoll_api;
#endif

#ifdef EVENT_HAVE_KQUEUE
extern const event_op kqueue_api;
#endif

#ifdef EVENT_HAVE_SELECT
extern const event_op select_api;
#endif

const event_op *event_ops[] =
{
#ifdef EVENT_HAVE_EPOLL
    &epoll_api,
#endif

#ifdef EVENT_HAVE_KQUEUE
    &kqueue_api,
#endif

#ifdef EVENT_HAVE_SELECT
    &select_api,
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
    event_log *log;
    
    loop->set_size = EVENT_LOOP_INITIAL_SIZE;
    loop->max_size = EVENT_LOOP_MAX_SIZE;

    log = event_log_create();
    if(log == NULL)
    {
        LOG("event_log_create");
        return -1;
    }
    event_log_init(log, PATH_DEFAULT, writer_default, LEVEL_DEFAULT);

    loop->registered_events = (event *)malloc(sizeof(event) * loop->set_size);
    if(NULL == loop->registered_events)
    {
        event_log_debug3(log, EMERG, "loop init:%s, file:%s, line:%d\n", strerror(errno), __FILE__, __LINE__);
        event_log_destroy(log);
        return -1;
    }

    loop->ready_events = (event *)malloc(sizeof(event) * loop->set_size);
    if(NULL == loop->ready_events)
    {
        event_log_debug3(log, EMERG, "loop init:%s, file:%s, line:%d\n", strerror(errno), __FILE__, __LINE__);
        event_log_destroy(log);
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
        free(loop->registered_events);
        free(loop->ready_events);
        event_log_debug3(log, EMERG, "loop init:%s, file:%s, line:%d\n", strerror(errno), __FILE__, __LINE__);
        event_log_destroy(log);
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

    assert(api != NULL);

    if(ev->event_type & EVENT_TIMEOUT == 0)
    {
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
    else
    {
        int64_t now;
        
        now = get_time_miliseconds();
        ev->timeout += now;
        __add_event_to_timeout_queue(loop, ev);
        return 0;
    }
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
        event_log_debug3(loop->log, EMERG, "%s,%s,%d", strerror(errno), __FILE__, __LINE__);
        return -1;
    }

    registered = (event *)realloc(loop->registered_events, sizeof(event) * set_size);
    if(registered == NULL)
    {
        event_log_debug3(loop->log, EMERG, "event_loop_resize:%s, file:%s, line:%d\n", strerror(errno), __FILE__, __LINE__);
        return -1;
    }
    ready = (event *)realloc(loop->ready_events, sizeof(event) * set_size);
    if(ready == NULL)
    {
        event_log_debug3(loop->log, EMERG, "event_loop_resize:%s, file:%s, line:%d\n", strerror(errno), __FILE__, __LINE__);
        return -1;
    }

    ret = loop->api->event_loop_resize(loop, set_size);
    if(ret == -1)
    {
        /*realloc does not need free here*/
        event_log_debug3(loop->log, EMERG, "event_loop_resize:%s, file:%s, line:%d\n", strerror(errno), __FILE__, __LINE__);
        return -1;
    }

    loop->registered_events = registered;
    loop->ready_events = ready;
    loop->set_size = set_size;
    return 0;
}

void event_loop_main(event_loop *loop)
{
    int i;
    const event_op *api = loop->api;
    while(!loop->stop)
    {
        uint64_t timeout;
        __get_first_timeout(loop);
        timeout = loop->timeout;
        if(timeout != -1)
        {
            timeout -= get_time_miliseconds();
            if(timeout < 0)
            {
                timeout = -1;
            }
        }

        int events_num = api->event_loop_main(loop, timeout);
        for(i = 0; i < events_num; i++)
        {
            int fd = loop->ready_events[i].event_fd;
            event *e = &loop->registered_events[fd];
            event *r = &loop->ready_events[i];

            if(r->event_type & EVENT_ERROR)
            {
                //handle the erro
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
        __process_timeout_events(loop);
    } 
}

void __process_timeout_events(event_loop *loop)
{
    event *curr, *next;
    int64_t now;
    
    if(list_empty(&loop->timeout_queue))
    {
        return;
    }
    list_for_each_entry_safe(curr, next, (&(loop->timeout_queue)), timeout_next)
    {
        now = get_time_miliseconds();
        if(curr->timeout <= now)
        {
            list_del(&(curr->timeout_next));
            curr->event_cb(-1, EVENT_TIMEOUT, curr->event_args);
        }
        else
        {
            break;
        }

    }

}

void __add_event_to_timeout_queue(event_loop *loop, event *ev)
{
    event *p;
    int flag = 0;
    
    if(list_empty(&loop->timeout_queue))
    {
        list_add_tail(&ev->timeout_next, &loop->timeout_queue);
    }
    else
    {
        list_for_each_entry(p, (&(loop->timeout_queue)), timeout_next)
        {
            if(p->timeout > ev->timeout)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 0) /*should we insert ev to the tail*/
        {
            list_add_tail(&ev->timeout_next, &loop->timeout_queue);
        }
        else
        {
            list_add_tail(&(ev->timeout_next), &(p->timeout_next));
        }
    }
}

void __get_first_timeout(event_loop *loop)
{
    if(list_empty(&loop->timeout_queue))
    {
        loop->timeout = -1;
    }
    else
    {
        event *e = list_first_entry((&(loop->timeout_queue)), event, timeout_next);
        loop->timeout = e->timeout;
    }
}
