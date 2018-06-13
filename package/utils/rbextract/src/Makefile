CC = gcc
CFLAGS = -Wall
OBJS = rbextract.o rle.o minilzo.c

all: rbextract

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

rbextract: $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f rbextract *.o
