# Makefile to build lxdialog package
#

all: lxdialog

# Use reursively expanded variables so we do not call gcc unless
# we really need to do so. (Do not call gcc as part of make mrproper)
CFLAGS := $(shell sh check-lxdialog.sh -ccflags)
LIBS := $(shell sh check-lxdialog.sh -ldflags gcc)

# workaround for OpenBSD, which does not use symlinks to libncurses.so
OS := $(shell uname -s)
ifeq ($(strip $(OS)),OpenBSD)
LIBS := -lncurses
endif


always		:= $(hostprogs-y) dochecklxdialog

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

lxdialog: checklist.o menubox.o textbox.o yesno.o inputbox.o util.o lxdialog.o msgbox.o
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -f *.o lxdialog
