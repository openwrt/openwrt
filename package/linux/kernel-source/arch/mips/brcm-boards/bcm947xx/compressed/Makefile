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
# $Id$
#
# Copyright 2004  Manuel Novoa III <mjn3@codepoet.org>
#   Modified to support bzip'd kernels.
#   Of course, it would be better to integrate bunzip capability into CFE.
#

# Link at 3 MB offset in RAM
#TEXT_START	?= 0x80300000
TEXT_START	?= 0x80001000
BZ_MEM_TOP	:= 0x81000000
BZ_TEXT_START	:= BZ_MEM_TOP-0x4000
BZ_STACK_TOP	:= BZ_TEXT_START-4

OBJCOPY		:= $(CROSS_COMPILE)objcopy -O binary -R .reginfo -R .note -R .comment -R .mdebug -S

SRCBASE		:= $(TOPDIR)/../..
VPATH		:= $(SRCBASE)/shared
ASFLAGS		+= -D__ASSEMBLY__ -I$(SRCBASE)/include -DLOADADDR=$(LOADADDR)
ASFLAGS		+= -DBZ_MEM_TOP=$(BZ_MEM_TOP)
ASFLAGS		+= -DBZ_TEXT_START=$(BZ_TEXT_START)
ASFLAGS		+= -DBZ_STACK_TOP=$(BZ_STACK_TOP)
CFLAGS		+= -I$(SRCBASE)/include -DLOADADDR=$(LOADADDR)
CFLAGS		+= -DBZ_MEM_TOP=$(BZ_MEM_TOP)
CFLAGS		+= -DBZ_TEXT_START=$(BZ_TEXT_START)
CFLAGS		+= -DBZ_STACK_TOP=$(BZ_STACK_TOP)
ifdef CONFIG_MCOUNT
CFLAGS		:= $(subst -pg,,$(CFLAGS))
endif
CFLAGS		+= -ffunction-sections $(call check_gcc, -fvtable-gc, )
SEDFLAGS	:= s/BZ_TEXT_START/$(BZ_TEXT_START)/;s/BZ_MEM_TOP/$(BZ_MEM_TOP)/;s/TEXT_START/$(TEXT_START)/

SYSTEM		?= $(TOPDIR)/vmlinux
OBJECTS		:= head.o data.o

all: bzImage vmlinuz

# Don't build dependencies, this may die if $(CC) isn't gcc
dep:

# Create a gzipped version named vmlinuz for compatibility
vmlinuz: piggy
	gzip -c9 $< > $@

# Our bzImage is a gzip'd binary that decompresses and runs
#   the appended bzip'd kernel.
bzImage: bzLoaderImage.gz piggz
	cat bzLoaderImage.gz piggz > $@

bzLoaderImage.gz: bzLoaderImage
	gzip -nc9 $< > $@

bzLoaderImage: bzLoader
	$(OBJCOPY) $< $@

bzLoader: vmlinux.lds $(OBJECTS)
	$(LD) -static --gc-sections -no-warn-mismatch -T vmlinux.lds -o $@ $(OBJECTS)

vmlinux.lds: vmlinux.lds.in Makefile
	@sed "$(SEDFLAGS)" < $< > $@

piggz: piggy
	bzip2 -c9 $< > $@

piggy: $(SYSTEM)
	$(OBJCOPY) $< $@

data.o: data.lds data.image
	$(LD) -no-warn-mismatch -T data.lds -r -o $@ -b binary data.image -b elf32-tradlittlemips

data.lds:
	@echo "SECTIONS { .data : { code_start = .; *(.data) code_stop = .; }}" > $@

data.image: decompress_bunzip2.image
	$(OBJCOPY) $< $@

decompress_bunzip2.image: decompress_bunzip2.lds decompress_bunzip2.o
	$(LD) -static --gc-sections -no-warn-mismatch -T decompress_bunzip2.lds -o $@ decompress_bunzip2.o

decompress_bunzip2.lds: decompress_bunzip2.lds.in Makefile
	@sed "$(SEDFLAGS)" < $< > $@

mrproper: clean

clean:
	rm -f vmlinux vmlinuz piggz piggy *.lds *.o \
		bzLoader bzLoaderImage bzLoaderImage.gz bzImage \
		data.lds data.image \
		decompress_bunzip2.lds decompress_bunzip2.image
