# SPDX-License-Identifier: GPL-2.0-only
ARCH:=mips
SUBTARGET:=rtl930x
CPU_TYPE:=24kc
BOARDNAME:=Realtek MIPS RTL930X

define Target/Description
	Build firmware images for Realtek RTL930x based boards.
endef

include $(TOPDIR)/target/linux/realtek/switch.mk
