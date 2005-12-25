# $Id$
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

KERNEL_VERSION = $(shell uname -r | cut -d. -f1,2)

obj-m	:= switch-core.o switch-adm.o switch-robo.o

ifeq ($(KERNEL_VERSION),2.4)

O_TARGET := $(obj-m)

-include $(TOPDIR)/Rules.make

endif

all: modules

modules:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd) TOPDIR="$(KERNEL_DIR)" modules
