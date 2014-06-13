#ifndef _SIMPLE_EVENT_LOOP_H
#define _SIMPLE_EVENT_LOOP_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include "config.h"
#include "list.h"
#include "log.h"
#include "simple_event.h"


#define EVENT_LOOP_INITIAL_SIZE 256
#define EVENT_LOOP_MAX_SIZE 4096

struct event_loop;
/**
 * The multiplexed I/O interface
 */
typedef struct event_op
{
    const char *name; 
    int (*event_loop_init)(struct event_loop *);
    int (*event_register)(struct event_loop *, int, short);
    int (*event_unregister)(struct event_loop *,int, short);
    int (*event_loop_resize)(struct event_loop *, int);
    int (*event_loop_main)(struct event_loop *);
}event_op;

/**
 * The Event Loop for event reactor
 */
typedef struct event_loop
{
    const event_op *api;
    void *api_data;
    int set_size;
    int max_size;
    struct event *registered_events;
    struct event *ready_events;
    struct list_head timeout_queue;
    int stop;
}event_loop;

/*event loop interfaces*/
event_loop *event_loop_create(void);
int event_loop_init(event_loop *);
int event_register(event_loop *, event *);
int event_unregister(event_loop *, event *);
int event_loop_resize(event_loop *, int);
void event_loop_main(event_loop *);
void event_loop_destory(event_loop *loop);

#endif
