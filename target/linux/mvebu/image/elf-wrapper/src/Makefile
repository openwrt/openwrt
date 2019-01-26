#
# Copyright (C) 2019 Tobias Schramm
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

PKG_NAME := lzma-loader
PKG_BUILD_DIR := $(KDIR)/$(PKG_NAME)

WRAPPER_LOADADDR	?= 0x80001000

RM			?= rm -rf

CC			:= $(CROSS_COMPILE)gcc
LD			:= $(CROSS_COMPILE)ld
OBJCOPY			:= $(CROSS_COMPILE)objcopy

OBJFLAGS		:= -O elf32-littlearm --binary-architecture arm --section-alignment 4096
LDFLAGS			:= -m armelf_linux_eabi
CCFLAGS			:= -DWRAPPER_LOADADDR=$(WRAPPER_LOADADDR)

.PHONY: clean all

all: wrapper.elf

wrapper.lds: wrapper.lds.S
	$(CC) -E -P $(CCFLAGS) -o $@ $<

zImage.o: zImage
	$(OBJCOPY) $(OBJFLAGS) -I binary --set-section-flags .data=alloc,load,code $< $@

wrapper.elf: zImage.o wrapper.lds
	$(LD) $(LDFLAGS) -T wrapper.lds -o $@ $<

clean:
	$(RM) *.elf *.o *.lds
