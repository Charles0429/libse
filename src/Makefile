se.test: simple_event_loop.o simple_event.o epoll.o
	gcc -o se.test simple_event_loop.o simple_event.o epoll.o
simple_event_loop.o: simple_event_loop.c
	gcc -o simple_event_loop.o simple_event_loop.c
simple_event.o: simple_event.c
	gcc -o simple_event.o simple_event.c
epoll.o: epoll.c
	gcc -o epoll.o epoll.c

