#include <stdio.h>
#include "../src/se.h"

int main(void)
{
    event_log *log;
    log = event_log_create();
    if(log == NULL)
    {
        return -1;
    }

    event_log_init(log, PATH_DEFAULT, writer_default, LEVEL_DEFAULT);
    errno = ERANGE;
    event_log_debug3(log, DEBUG,  "%s,%s,%d", strerror(errno), __FILE__, __LINE__);    
}
