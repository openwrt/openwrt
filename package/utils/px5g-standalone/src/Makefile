CFLAGS?=-O2
CFLAGS+=
SFLAGS:=--std=gnu99
WFLAGS:=-Wall -Werror -pedantic
LDFLAGS?=
BINARY:=px5g

all: $(BINARY)

$(BINARY): *.c library/*.c
	$(CC) -I. $(CFLAGS) $(SFLAGS) $(WFLAGS) $(LDFLAGS) -o $@ $+

clean:
	rm -f $(BINARY)
