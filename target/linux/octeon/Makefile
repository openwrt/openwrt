#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=octeon
BOARDNAME:=Cavium Networks Octeon
FEATURES:=squashfs jffs2 pci usb
CFLAGS:=-Os -pipe -march=octeon -fno-caller-saves
MAINTAINER:=Florian Fainelli <florian@openwrt.org>

LINUX_VERSION:=2.6.37.6

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += wpad-mini

define Target/Description
	Build firmware images for Cavium Networks Octeon-based boards.
endef

$(eval $(call BuildTarget))
