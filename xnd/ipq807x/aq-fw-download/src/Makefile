ifndef CFLAGS
CFLAGS = -O2 -g
endif
INCLUDES=-Iinclude -Iinclude/registerMap \
	 -Iinclude/registerMap/APPIA \
	 -Iinclude/registerMap/HHD

all: aq-fw-download

%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $^

aq-fw-download: mdioBootLoadCLD.o src/AQ_PhyInterface.o src/AQ_API.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
