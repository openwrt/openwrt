#	Copyright (C) 2004 - 2007 rt2x00 SourceForge Project
#	<http://rt2x00.serialmonkey.com>
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program; if not, write to the
#	Free Software Foundation, Inc.,
#	59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#	Module: Makefile
#	Abstract: Makefile for rt2x00 kernel module

#
# Set the enviroment variables.
#
ifndef SUBDIRS
	SUBDIRS=$(shell pwd)
endif

ifdef KERNDIR
	KERNEL_SOURCES := $(KERNDIR)
else
	KERNEL_SOURCES := /lib/modules/$(shell uname -r)/build
endif

ifdef KERNOUT
	KERNEL_OUTPUT := KBUILD_OUTPUT=$(KERNOUT)
else
	KERNEL_OUTPUT :=
endif

#
# Include kernel and rt2x00 config.
#
include $(KERNEL_SOURCES)/.config
include $(SUBDIRS)/config

#
# Determine if and with what options the rt2x00 drivers should be build
#
rt2x00lib-objs := rt2x00dev.o rt2x00mac.o

ifeq ($(CONFIG_RT2X00),y)

ifeq ($(CONFIG_RT2X00_LIB_DEBUGFS),y)
	rt2x00lib-objs += rt2x00debug.o
endif

ifeq ($(CONFIG_RT2400PCI),y)
	obj-m += rt2400pci.o rt2x00pci.o rt2x00lib.o
ifeq ($(CONFIG_RT2400PCI_RFKILL),y)
	rt2x00lib-objs += rt2x00rfkill.o
	CFLAGS += -DCONFIG_RT2X00_LIB_RFKILL
endif
endif

ifeq ($(CONFIG_RT2500PCI),y)
	obj-m += rt2500pci.o rt2x00pci.o rt2x00lib.o
ifeq ($(CONFIG_RT2500PCI_RFKILL),y)
	rt2x00lib-objs += rt2x00rfkill.o
	CFLAGS += -DCONFIG_RT2X00_LIB_RFKILL
endif
endif

ifeq ($(CONFIG_RT2500USB),y)
	obj-m += rt2500usb.o rt2x00usb.o rt2x00lib.o
endif

ifeq ($(CONFIG_RT61PCI),y)
	CFLAGS += -DCONFIG_RT2X00_LIB_FIRMWARE
	rt2x00lib-objs += rt2x00firmware.o
	obj-m += rt61pci.o rt2x00pci.o rt2x00lib.o
ifeq ($(CONFIG_RT61PCI_RFKILL),y)
	rt2x00lib-objs += rt2x00rfkill.o
	CFLAGS += -DCONFIG_RT2X00_LIB_RFKILL
endif
endif

ifeq ($(CONFIG_RT73USB),y)
	CFLAGS += -DCONFIG_RT2X00_LIB_FIRMWARE
	rt2x00lib-objs += rt2x00firmware.o
	obj-m += rt73usb.o rt2x00usb.o rt2x00lib.o
endif

endif

MAKEFLAGS += --no-print-directory
CFLAGS := -include $(SUBDIRS)/rt2x00_compat.h $(CFLAGS)

all: default

config_header:
	@if [ ! -f "rt2x00_config.h" ] || [ "rt2x00_config.h" -ot "config" ]; \
	then \
		awk -F = > rt2x00_config.h < config '/^CONFIG.*$\/ \
		{ \
			if($$2 == "y") { \
				print "#ifndef " $$1; \
				print "#define " $$1; \
				print "#endif"; \
				print "" \
			} else { \
				print "#undef " $$1; \
				print ""; \
			} \
		}'; \
	fi

default: config_header
	@$(MAKE) -C $(KERNEL_SOURCES) SUBDIRS=$(SUBDIRS) $(KERNEL_OUTPUT) \
		modules

sparse: config_header
	@$(MAKE) -C $(KERNEL_SOURCES) SUBDIRS=$(SUBDIRS) $(KERNEL_OUTPUT) \
		modules C=1 CF=-D__CHECK_ENDIAN__

install: config_header
	@$(MAKE) -C $(KERNEL_SOURCES) SUBDIRS=$(SUBDIRS) $(KERNEL_OUTPUT) \
		INSTALL_MOD_DIR=rt2x00 $(KERNEL_OUTPUT) modules_install
	/sbin/depmod -a

clean:
	@rm -f rt2x00_config.h
	@rm -f Modules.symvers Module.symvers
	@for folder in $(EXTMODDIRS); \
	do \
		rm -f $${folder}/*.o \
		rm -f $${folder}/*.ko \
		rm -f $${folder}/*.s \
		rm -f $${folder}/*.mod.c \
		rm -f $${folder}/.*.cmd \
		rm -f $${folder}/.*.flags \
		rm -f $${folder}/.*.o.d \
		rm -f $${folder}/.*.s.d \
		rm -f $${folder}/.#* \
		rm -f $${folder}/*~ \
		rm -fr $${folder}/.tmp_versions; \
	done
