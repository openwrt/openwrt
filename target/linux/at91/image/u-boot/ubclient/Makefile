#

all: ubpar

crc32.c:
	ln -s ../lib_generic/crc32.c ./

%.o: %.c
	$(CC) -I ../include $(CFLAGS) $(EXTRA_FLAGS) -DLAN_IP=$(LAN_IP) -DLAN_SERVERIP=$(LAN_SERVERIP) -c -o $@ $^

ubpar: ubpar.o crc32.o
	$(CC) -o $@ $^

clean:
	rm -f *.o ubpar
