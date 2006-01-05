VERSION = 0.0.1

FIRMWARE_INSTALL_DIR ?= /lib/firmware

CC = cc
PREFIX = /usr/local
CFLAGS = -std=c99 -O2 -fomit-frame-pointer -Wall -pedantic -D_BSD_SOURCE
LDFLAGS =


OBJECTS = fwcutter.o md5.o

CFLAGS += -DFWCUTTER_VERSION_=$(VERSION)

all: fwcutter

fwcutter: $(OBJECTS)
	$(CC) $(CFLAGS) -o fwcutter $(OBJECTS) $(LDFLAGS)

install: all
	-install -o 0 -g 0 -m 755 fwcutter $(PREFIX)/bin/

clean:
	-rm -f *~ *.o *.orig *.rej *.fw fwcutter

installfw:
	-if ! [ -d $(FIRMWARE_INSTALL_DIR) ]; then mkdir $(FIRMWARE_INSTALL_DIR); fi
	-install -o 0 -g 0 -m 600 bcm43xx_*.fw $(FIRMWARE_INSTALL_DIR)

# dependencies
fwcutter.o: md5.h fwcutter_list.h
md5.o: md5.h
