#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=ar71xx
BOARDNAME:=Atheros AR71xx
FEATURES:=squashfs tgz broken

LINUX_VERSION:=2.6.26.5

include $(INCLUDE_DIR)/target.mk

#DEFAULT_PACKAGES += kmod-madwifi

define Target/Description
	Build firmware images for Atheros AR71xx based boards
endef

$(eval $(call BuildTarget))
