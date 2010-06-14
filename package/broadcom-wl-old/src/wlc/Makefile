all: wlc
clean:
	rm -f *.o wlc

wlc: wlc.o ioctl.o
%.o: %.c
	$(CC) $(CFLAGS) -I../include -Wall -c -o $@ $<

.PHONY: all clean
