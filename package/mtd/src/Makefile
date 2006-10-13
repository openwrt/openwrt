# $Id$

all: mtd

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $^

mtd: mtd.o
	$(CC) -o $@ $^

clean:
	rm -f *.o mtd
