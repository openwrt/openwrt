# SPDX-License-Identifier: GPL-2.0-only
ARCH:=mips
SUBTARGET:=rtl931x
CPU_TYPE:=24kc
BOARDNAME:=Realtek MIPS RTL931X

define Target/Description
	Build firmware images for Realtek RTL931x based boards.
endef

include $(TOPDIR)/target/linux/realtek/switch.mk
