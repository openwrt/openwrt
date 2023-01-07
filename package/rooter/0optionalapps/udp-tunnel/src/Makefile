prefix = /usr/local

CFLAGS ?= -g -O2

INSTALL ?= install
PKG_CONFIG ?= pkg-config

ifeq ($(shell $(PKG_CONFIG) --exists libsystemd || echo NO),)
DEFS += -DHAVE_SYSTEMD_SD_DAEMON_H $(shell $(PKG_CONFIG) --cflags libsystemd)
LDADD += $(shell $(PKG_CONFIG) --libs libsystemd)
endif

CPPFLAGS += $(DEFS) $(INCLUDES)

OBJECTS := log.o network.o utils.o udptunnel.o

all: depend udptunnel

install:
	$(INSTALL) -d $(BASEDIR)$(prefix)/sbin/
	$(INSTALL) -m 0755 udptunnel $(BASEDIR)$(prefix)/sbin/

clean:
	rm -f Makefile.depend $(OBJECTS) udptunnel

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

udptunnel: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDADD) $(LIBS)

depend: Makefile.depend
Makefile.depend:
	$(CC) $(CPPFLAGS) $(CFLAGS) -MM -MG *.c > $@

-include Makefile.depend
