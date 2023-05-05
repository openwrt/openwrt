# SPDX-License-Identifier: GPL-2.0-only
ARCH:=mips
SUBTARGET:=rtl839x
CPU_TYPE:=24kc
BOARDNAME:=Realtek MIPS RTL839X

define Target/Description
	Build firmware images for Realtek RTL839x based boards.
endef

include $(TOPDIR)/target/linux/realtek/switch.mk
