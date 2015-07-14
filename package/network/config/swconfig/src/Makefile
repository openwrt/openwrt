ifndef CFLAGS
CFLAGS = -O2 -g -I ../src
endif
LIBS=-lnl -lnl-genl

all: swconfig

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $^

libsw.so: swlib.o
	$(CC) $(CFLAGS) -fPIC -shared -o $@ swlib.o

swconfig: libsw.so cli.o uci.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) -L./ -lsw
