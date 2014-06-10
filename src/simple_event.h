#ifndef _SIMPLE_EVENT_H
#define _SIMPLE_EVENT_H

#include <stdlib.h>
#include "list.h"

#define EVENT_ERROR 0X80
#define EVENT_READ 0x01
#define EVENT_WRITE 0x02
#define EVENT_TIMEOUT 0x04


typedef void (*event_callback)(int event_fd, short event_type, void *event_args);

/**
 * Event Struct for event handle
 *
 */
typedef struct event
{
    int event_fd;
    short event_type;
    void *event_args;
    event_callback event_cb;

    struct list_head timeout_next;
}event;

/*event interfaces*/
event *event_create(void);
void event_set(struct event *, int, short, event_callback,  void *);
void event_destory(struct event *);

#endif
