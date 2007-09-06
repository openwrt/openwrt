#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=adm5120eb
BOARDNAME:=ADM5120 (Big Endian)
FEATURES:=squashfs pci usb broken

LINUX_VERSION:=2.6.22.4

define Target/Description
	Build firmware images for Infineon/ADMTek ADM5120 based boards running in big-endian mode
	(e.g : ZyXEL Prestige 335WT ...)
endef

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += admswconfig

# include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))
