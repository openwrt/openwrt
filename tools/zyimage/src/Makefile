WFLAGS = -Wall
zyimage-objs = zyimage.o

all: zyimage

%.o: %.c
	$(CC) $(CFLAGS) $(WFLAGS) -c -o $@ $<

zyimage: $(zyimage-objs)
	$(CC) $(LDFLAGS) -o $@ $(zyimage-objs)

clean:
	rm -f zyimage *.o
