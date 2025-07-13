#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=sf16a18-fullmask
BOARDNAME:=sf16a18 full mask based boards
ARCH_PACKAGES:=mips_siflower
CPU_TYPE:=mips-interAptiv

define Target/Description
	Build firmware images for siflower sf16a18 full mask based boards.
endef
