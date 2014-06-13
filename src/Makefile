CC=gcc
CCFLAGS=-Wall
LDFLAGS=-fPIC -shared


libse.so: 
	$(CC) simple_event_loop.c epoll.c select.c simple_event.c $(LDFLAGS) -o libse.so
