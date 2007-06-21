# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=brcm47xx
BOARDNAME:=Broadcom BCM947xx/953xx
FEATURES:=squashfs usb

LINUX_VERSION:=2.6.22-rc5
LINUX_KERNEL_MD5SUM:=50dda556ae492f68ff7c273e1f357c55

define Target/Description
	Build firmware images for Broadcom based routers
	(e.g. Netgear WGT634U)
endef

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += kmod-switch kmod-diag

# include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))
