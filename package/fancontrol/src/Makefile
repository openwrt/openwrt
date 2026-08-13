CC         := gcc
CFLAGS     := -O3 -Wall -fomit-frame-pointer

all: fancontrol

fancontrol: %: %.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

clean:
	rm -f fancontrol *.o *~