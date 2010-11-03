ifndef CFLAGS
CFLAGS = -O2 -g -I ../src
endif

FPIC=-fPIC

all: libtapi.so

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(FPIC)

TAPI_OBJS = \
	timer_fd.o \
	events.o \
	tapi-port.o \
	tapi-device.o \
	tapi-session.o \
	tapi-stream.o

tapidemo: tapidemo.o libtapi.so
	$(CC) $(LDFLAGS) -o $@ $^

libtapi.so: $(TAPI_OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(FPIC)

clean:
	rm -rf *.o *.so
