#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=rt288x
BOARDNAME:=RT288x based boards
CFLAGS+= -mips32r2 -mtune=mips32r2

define Target/Description
	Build firmware images for Ralink RT288x based boards.
endef

