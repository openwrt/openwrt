include ../Makefile.inc

CPPFLAGS += -I../kernel
LDFLAGS =

ifneq ($(HOST_OS),Linux)
USE_LIBNL_MICRO=1
else
USE_LIBNL_MICRO=
endif

ifeq ($(USE_LIBNL_MICRO),1)
LIBNL_PREFIX = /usr/local
LIBNL = $(LIBNL_PREFIX)/lib/libnl-micro.a
CPPFLAGS += -I$(LIBNL_PREFIX)/include/libnl-micro
EXTRA_CFLAGS += -DNO_LOCAL_ACCESS
else
LIBNL = -lnl
endif

LIBM = -lm
LIBS = $(LIBNL) $(LIBM)

all: libwprobe.a wprobe-util

libwprobe.a: wprobe-lib.o
	rm -f $@
	$(AR) rcu $@ $^
	$(RANLIB) $@

%.o: %.c
	$(CC) $(WFLAGS) -c -o $@ $(CPPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $<

wprobe-util: wprobe-util.o wprobe-lib.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o *.a wprobe-util
