#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=octeon
BOARDNAME:=Cavium Networks Octeon
FEATURES:=squashfs jffs2 pci
CFLAGS:=-Os -pipe -mtune=octeon -funit-at-a-time

LINUX_VERSION:=2.6.30.7

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += hostapd-mini

define Target/Description
	Build firmware images for Cavium Networks Octeon-based boards.
endef

$(eval $(call BuildTarget))
