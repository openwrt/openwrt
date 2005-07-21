# $Id$

EXTRA_CFLAGS := -c -I. -I../include
LIBSHARED_OBJS := shutils.o wl.o wl_linux.o defaults.o linux_timer.o
LIBNVRAM_OBJS := nvram_linux.o nvram_convert.o

all: libshared.so libnvram.so nvram

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $^

libshared.so: $(LIBSHARED_OBJS)
	$(CC) -shared -o $@ $^

libnvram.so: $(LIBNVRAM_OBJS)
	$(CC) -shared -o $@ $^

nvram: main.o
	$(CC) -o $@ $^ -L. -lnvram

clean:
	rm -f *.o *.so nvram
