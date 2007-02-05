# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=brcm
BOARDNAME:=Broadcom BCM947xx/953xx
FEATURES:=squashfs

define Target/Description
	Build firmware images for Broadcom based routers
	(e.g. Netgear WGT634U)
endef

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += kmod-switch kmod-diag

define Profile/Atheros
  NAME:=Atheros WiFi (default)
  PACKAGES:=kmod-madwifi
endef

define Profile/Atheros/Description
	Package set compatible with hardware using Atheros WiFi cards
endef
$(eval $(call Profile,Atheros))

define Profile/WGT634U
  NAME:=Netgear WGT634U
  PACKAGES:=kmod-madwifi kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/WGT634U/Description
	Package set compatible with the Netgear WGT634U. Contains USB support
endef
$(eval $(call Profile,WGT634U))

define Profile/None
  NAME:=No WiFi
  PACKAGES:=
endef

define Profile/None/Description
	Package set without WiFi support
endef
$(eval $(call Profile,None))

$(eval $(call BuildKernel))
