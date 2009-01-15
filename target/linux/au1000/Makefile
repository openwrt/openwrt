#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=au1000
BOARDNAME:=RMI/AMD AU1x00
FEATURES:=jffs2 usb pci

LINUX_VERSION:=2.6.27.11

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += hostapd-mini yamonenv

define Target/Description
	Build firmware for AMD Alchemy 1500 boards
	(e.g. 4G-Systems Mesh/Access Cube ...)
endef

define Kernel/BuildImage
	$(call Kernel/BuildImage/Default)
	$(CP) $(LINUX_DIR)/arch/mips/boot/compressed/images/zImage.flash.srec $(LINUX_DIR)/zImage.flash.srec
	$(CP) $(LINUX_DIR)/arch/mips/boot/compressed/images/zImage.srec $(LINUX_DIR)/zImage.srec
endef

$(eval $(call BuildTarget))
