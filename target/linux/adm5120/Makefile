#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=adm5120
BOARDNAME:=ADM5120 (Little Endian)
FEATURES:=squashfs jffs2 tgz broken

LINUX_VERSION:=2.6.22.4

define Target/Description
	Build firmware images for Infineon/ADMtek ADM5120 based boards
	(e.g : RouterBoard RB1xx, Compex WP54G-WRT ...)
endef

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += admswconfig

# include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))
