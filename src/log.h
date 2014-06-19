#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>

#define MAX_LINE 2048

#define EMERG 0
#define ALERT 1
#define CRIT 2
#define ERR 3
#define WARNING 4
#define NOTICE 5
#define INFO 6
#define DEBUG 7

#define LEVEL_DEFAULT DEBUG
#define PATH_DEFAULT "/usr/local/event.log"

#define LOG(x) perror(x)

typedef int (*writer)(int, const void *, int);

struct event_log
{
    const char *path;
    int fd;
    writer write;
    int level;
};

typedef struct event_log event_log;

event_log *event_log_create(void);
int event_log_init(event_log *log, const char *path, writer write, int level);
void event_log_debug(event_log *log, int level, const char *fmt, ...);
void event_log_close(event_log *log);
void event_log_destroy(event_log *log);
int writer_default(int fd, const void *buf, int count);


#define event_log_debug1(log, level, fmt, arg1)                            \
        event_log_debug(log, level, fmt, arg1)

#define event_log_debug2(log, level, fmt, arg1, arg2)                      \
        event_log_debug(log, level, fmt, arg1, arg2)

#define event_log_debug3(log, level, fmt, arg1, arg2, arg3)                \
        event_log_debug(log, level, fmt, arg1, arg2, arg3)

#define event_log_debug4(log, level, fmt, arg1, arg2, arg3, arg4)          \
        event_log_debug(log, level, fmt, arg1, arg2, arg3, arg4)

#define event_log_debug5(log, level, fmt, arg1, arg2, arg3, arg4, arg5)    \
        event_log_debug(log, level, fmt, arg1, arg2, arg3, arg4, arg5)

#define event_log_debug6(log, level, fmt,                                  \
                       arg1, arg2, arg3, arg4, arg5, arg6)                    \
        event_log_debug(log, level, fmt,                                   \
                       arg1, arg2, arg3, arg4, arg5, arg6)

#define event_log_debug7(level, log, err, fmt,                                  \
                       arg1, arg2, arg3, arg4, arg5, arg6, arg7)              \
        event_log_debug(level, log, err, fmt,                                   \
                       arg1, arg2, arg3, arg4, arg5, arg6, arg7)

#define event_log_debug8(level, log, err, fmt,                                  \
                       arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)        \
        event_log_debug(level, log, err, fmt,                                   \
                       arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)



#endif
