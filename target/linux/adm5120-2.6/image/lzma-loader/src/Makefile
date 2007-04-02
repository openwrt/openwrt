#
# Makefile for Broadcom BCM947XX boards
#
# Copyright 2001-2003, Broadcom Corporation
# All Rights Reserved.
#
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# Copyright 2004  Manuel Novoa III <mjn3@codepoet.org>
#   Modified to support bzip'd kernels.
#   Of course, it would be better to integrate bunzip capability into CFE.
#
# Copyright 2005  Oleg I. Vdovikin <oleg@cs.msu.su>
#   Cleaned up, modified for lzma support, removed from kernel
#
# Copyright 2007 Gabor Juhos <juhosg@freemail.hu>
#   Modified to support user defined entry point address.
#   Added support for make targets with different names
#

LOADADDR	:= 0x80001000
BZ_TEXT_START	:= 0x80300000
BZ_STARTUP_ORG	:= 0
LOADER		:= loader

OBJCOPY		:= $(CROSS_COMPILE)objcopy -O binary -R .reginfo -R .note -R .comment -R .mdebug -S

CFLAGS		= -D__KERNEL__ -Wall -Wstrict-prototypes -Wno-trigraphs -Os \
		  -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic \
		  -ffunction-sections -pipe -mlong-calls -fno-common \
		  -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap
CFLAGS		+= -DLOADADDR=$(LOADADDR) -D_LZMA_IN_CB

ASFLAGS		= $(CFLAGS) -D__ASSEMBLY__ -DBZ_STARTUP_ORG=$(BZ_STARTUP_ORG)

LDFLAGS		= -static --gc-sections -no-warn-mismatch
LDFLAGS		+= -e startup -Ttext $(BZ_TEXT_START) -T loader.lds.in

OBJECTS		:= $(LOADER)-head.o decompress.o LzmaDecode.o

all: $(LOADER).gz $(LOADER).elf

# Don't build dependencies, this may die if $(CC) isn't gcc
dep:

install:

decompress.o:
	$(CC) $(CFLAGS) -c decompress.c -o $@

$(LOADER)-head.o:
	$(CC) $(ASFLAGS) -c head.S -o $@

$(LOADER).gz: $(LOADER).bin
	gzip -nc9 $< > $@

$(LOADER).elf: $(LOADER).o
	cp $< $@

$(LOADER).bin: $(LOADER).o
	$(OBJCOPY) -O binary $< $@

$(LOADER).o: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

mrproper: clean

clean:
	rm -f *.gz *.elf *.bin *.o
