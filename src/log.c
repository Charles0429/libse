#include "log.h"

event_log *event_log_create(void)
{
    event_log *log;
    
    log = (event_log *)malloc(sizeof(event_log));
    return log;
}

int event_log_init(event_log *log, const char *path, writer write, int level)
{
    int fd;

    assert(log != NULL);

    fd = open(path, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if(fd == -1)
    {
        LOG("open");
        return -1;
    }

    log->path = path;
    log->fd = fd;
    log->write = write;
    log->level = level;
    return 0;
}

void event_log_debug(event_log *log, int level, const char *fmt, ...)
{
    int cnt;
    char buf[MAX_LINE];
    va_list args;

    va_start(args, fmt);
    cnt = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if(log->level >= level)
    {
        log->write(log->fd, buf, cnt);
    }
}

void event_log_close(event_log *log)
{
    close(log->fd);
}

void event_log_destory(event_log *log)
{
    event_log_close(log);
    free(log);
}

int writer_default(int fd, const void *buf, int count)
{
    return write(fd, buf, count);
}
