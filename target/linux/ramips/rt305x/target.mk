#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=rt305x
BOARDNAME:=RT3x5x/RT5350 based boards
ARCH_PACKAGES:=ramips_24kec
FEATURES+=usb
CFLAGS+= -march=24kec -mdsp

define Target/Description
	Build firmware images for Ralink RT3x5x/RT5350 based boards.
endef

