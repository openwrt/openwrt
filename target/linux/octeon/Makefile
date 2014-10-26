#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips64
BOARD:=octeon
BOARDNAME:=Cavium Networks Octeon
FEATURES:=squashfs jffs2 ext4 pci usb
CPU_TYPE:=octeon
CPU_CFLAGS_octeon:=-march=octeon -mabi=64
MAINTAINER:=John Crispin <blogic@openwrt.org>

KERNEL_PATCHVER:=3.10

include $(INCLUDE_DIR)/target.mk


define Target/Description
	Build firmware images for Cavium Networks Octeon-based boards.
endef

$(eval $(call BuildTarget))
