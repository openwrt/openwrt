#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=xburst
BOARDNAME:=XBurst JZ47x0
FEATURES:=jffs2 tgz ubifs

LINUX_VERSION:=2.6.32.6

DEVICE_TYPE=other

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=uImage

DEFAULT_PACKAGES += gpioctl

define Target/Description
	Build firmware images for XBurst JZ47x0 based boards.
endef

$(eval $(call BuildTarget))
