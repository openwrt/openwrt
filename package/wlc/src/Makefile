all: wlc
clean:
	rm -f *.o wlc

wlc: wlc.o ioctl.o
%.o: %.c
	$(CC) $(CFLAGS) -Wall -c -o $@ $<

.PHONY: all clean
