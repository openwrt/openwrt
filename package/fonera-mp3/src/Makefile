PROGS     = mp3d

INSTDIR   = $(prefix)/usr/bin
INSTMODE  = 0755
INSTOWNER = root
INSTGROUP = root

OBJS = mp3_main.o lib/mp3_states.o lib/mp3_common.o lib/mp3_stream.o \
	lib/mp3_file.o lib/mp3_nix_socket.o lib/mp3_socket_parser.o \
	lib/mp3_misc.o lib/mp3_playtime.o lib/mp3_tcp_socket.o 

all: $(PROGS)
$(PROGS): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@
	$(STRIP) $@
	
install: $(PROGS)
	$(INSTALL) -d $(INSTDIR)
	$(INSTALL) -m $(INSTMODE) -o $(INSTOWNER) -g $(INSTGROUP) $(PROGS) $(INSTDIR)

clean:
	rm -f $(PROGS) *.o core
	rm -f lib/$(PROGS) lib/*.o liob/core

