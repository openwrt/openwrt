#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=rt305x
BOARDNAME:=RT305x based boards
ARCH_PACKAGES:=ramips_24kec
CFLAGS+= -march=24kec

define Target/Description
	Build firmware images for Ralink RT305x based boards.
endef

