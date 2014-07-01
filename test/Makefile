CC=gcc
LDFLAGS=-L . -lse

all: log.test event.test timeout.test

log.test:
	$(CC) log_test.c $(LDFLAGS) -o log.test

event.test:
	$(CC) event_test.c $(LDFLAGS) -o event.test

timeout.test:
	$(CC) timeout_test.c $(LDFLAGS) -o timeout.test

clean:
	rm -f log.test event.test timeout.test
