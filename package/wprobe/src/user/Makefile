CFLAGS = -O2
CPPFLAGS ?= -I../kernel
WFLAGS = -Wall -Werror
LDFLAGS =

LIBNL = -lnl
LIBM = -lm
LIBS = $(LIBNL) $(LIBM)

all: libwprobe.a wprobe-info

libwprobe.a: wprobe.o
	rm -f $@
	$(AR) rcu $@ $^
	$(RANLIB) $@

%.o: %.c
	$(CC) $(WFLAGS) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<

wprobe-info: wprobe-info.o wprobe.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)
