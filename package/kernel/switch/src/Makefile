#
# Makefile for switch driver
#
# Copyright (C) 2005 Felix Fietkau <nbd@openwrt.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

obj-m	:= switch-core.o switch-adm.o switch-robo.o

ifeq ($(MAKING_MODULES),1)
export-objs := switch-core.o

-include $(TOPDIR)/Rules.make
endif
