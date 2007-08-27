PROGS     = mp3.cgi

OBJS = main.o


all: $(PROGS)
$(PROGS): 
	$(CC) $(CFLAGS) $(LDFLAGS) $^  -o $(PROGS) main.c $(LDLIBS)
	$(STRIP) $(PROGS)
	
clean:
	rm -f $(PROGS) *.o core

