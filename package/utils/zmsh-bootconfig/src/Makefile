CFLAGS += -Wall

zmsh-bootconfig: zmsh-bootconfig.o
	$(CC) $(LDFLAGS) zmsh-bootconfig.o -o zmsh-bootconfig

zmsh-bootconfig.o: zmsh-bootconfig.c
	$(CC) $(CFLAGS) -c zmsh-bootconfig.c

clean:
	rm -f *.o zmsh-bootconfig
