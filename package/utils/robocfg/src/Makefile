
all: robocfg

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $^

robocfg: robocfg.o
	$(CC) -o $@ $^

clean:
	rm -f *.o robocfg
