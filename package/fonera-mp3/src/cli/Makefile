
PROGS     = mplay

INSTDIR   = $(prefix)/usr/bin
INSTMODE  = 0755
INSTOWNER = root
INSTGROUP = root

OBJS = main.o

all: $(PROGS)
$(PROGS): 
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o mplay main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o mplay_tcp main_tcp.c
	gcc -o mplay_tcp_native main_tcp.c
	$(STRIP) mplay
	$(STRIP) mplay_tcp
	
install: $(PROGS)
	$(INSTALL) -d $(INSTDIR)
	$(INSTALL) -m $(INSTMODE) -o $(INSTOWNER) -g $(INSTGROUP) $(PROGS) $(INSTDIR)
	$(INSTALL) -m $(INSTMODE) -o $(INSTOWNER) -g $(INSTGROUP) $(PROGS)_tcp $(INSTDIR)

clean:
	rm -f $(PROGS) *.o core

