#
# Copyright (C) 2011 OpenWrt.org
#

SUBTARGET:=rt3883
BOARDNAME:=RT3662/RT3883 based boards
ARCH_PACKAGES:=ramips_24kec
CFLAGS+= -march=24kec

define Target/Description
	Build firmware images for Ralink RT3662/RT3883 based boards.
endef

