#
# Makefile for wlcompat
#
# Copyright (C) 2005-2007 Felix Fietkau <nbd@openwrt.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

EXTRA_CFLAGS += -DBCMDRIVER=1 -DBCMDMA64=1 -I../include
ifneq ($(DEBUG),)
  EXTRA_CFLAGS += -DDEBUG=1
  _D=-debug
endif
ifneq ($(WL_WEXT),)
  EXTRA_CFLAGS += -DWL_WEXT=1
endif

obj-m := wlcompat$(_D).o
modules: wlcompat$(_D).o

wlcompat$(_D).o: wlcompat.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS_nostdinc) -DKBUILD_BASENAME=$(subst $(comma),_,$(subst -,_,$(*F))) $(CFLAGS_$@) -c -o $@ $<

-include $(TOPDIR)/Rules.make
