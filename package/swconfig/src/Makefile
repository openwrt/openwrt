ifndef CFLAGS
CFLAGS = -O2 -g -I ../src
endif
LIBS=-lnl -lnl-genl

all: swconfig

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

swconfig: cli.o swlib.o uci.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
