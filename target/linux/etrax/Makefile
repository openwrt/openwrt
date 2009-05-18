#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=cris
BOARD:=etrax
BOARDNAME:=Foxboard (ETRAX 100LX)
FEATURES:=squashfs jffs2
LINUX_VERSION:=2.6.30-rc6
DEVICE_TYPE=

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="zImage"

define Target/Description
	Build firmware images for the FOXBOARD made by acmesystems.it
endef

$(eval $(call BuildTarget))
