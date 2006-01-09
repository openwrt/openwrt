# $Id$

EXTRA_CFLAGS := -c -I.
PIPACS_OBJS := pipacs.o parser.o

all: pipacs

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $^

pipacs: $(PIPACS_OBJS)
	$(CC) -o $@ $^

clean:
	rm -f *.o *.so pipacs
