#
# Makefile for Gemin kernel copy stub
#
# Copyright (C) 2019 Linus Walleij <linus.walleij@linaro.org>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#

AS		:= $(CROSS_COMPILE)as
OBJCOPY		:= $(CROSS_COMPILE)objcopy

BIN_FLAGS	:= -O binary -S

all: copy-kernel.bin

# Don't build dependencies, this may die if $(CC) isn't gcc
dep:

install:

%.o : %.S
	$(AS) $(ASFLAGS) -k -o $@ $<

%.bin: %.o
	$(OBJCOPY) $(BIN_FLAGS) $< $@

mrproper: clean

clean:
	rm -f copy-kernel.bin copy-kernel.o
