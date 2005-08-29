## Wi-viz makefile
# Supply your own C cross-compiler; I recommend the one from the OpenWRT buildroot
# Also requires a libpcap to link with, use libpcap.a for static, .so for shared
CC=~/buildroot/staging_dir_mipsel/bin/mipsel-linux-gcc
LDFLAGS=-L~/buildroot/staging_dir_mipsel/lib
LIBS=-lpcap

CCOPTS=-O2 -Os -pipe -mips32 -mtune=mips32
INCLUDE=-I~/buildroot/staging_dir_mipsel/include
SOURCES=wiviz.c wl_access.c channelhopper.c
OBJS=wiviz.o wl_access.o channelhopper.o
TARGET=wiviz

wiviz: ${OBJS}
	${CC} ${CCOPTS} ${INCLUDE} -o ${TARGET} ${OBJS} ${LDFLAGS} ${LIBS}

wiviz.o: wiviz.c
	${CC} ${CCOPTS} ${INCLUDE} -c ${SOURCES}
wl_access.o: wl_access.c
	${CC} ${CCOPTS} ${INCLUDE} -c ${SOURCES}
channelhopper.o: channelhopper.c
	${CC} ${CCOPTS} ${INCLUDE} -c ${SOURCES}

remake:
	touch wiviz.c wl_access.c channelhopper.c
