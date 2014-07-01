CC=gcc
CCFLAGS=-Wall
LDFLAGS=-fPIC -shared


libse.so: 
	$(CC) simple_event_loop.c epoll.c select.c simple_event.c log.c time.c $(LDFLAGS) -o libse.so

clean:
	rm -f libse.so
