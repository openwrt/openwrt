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
	(e.g. Linksys WRT54G(S), Asus WL-500g, Motorola WR850G)

	This one builds the generic .trx images, activate one of the
	options below for creating device specific files for use
	with TFTP client too.
endef

KERNEL:=2.4

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += kmod-switch kmod-diag nvram

define Profile/Broadcom
  NAME:=Generic, Broadcom WiFi (default)
  PACKAGES:=kmod-brcm-wl wlc nas kmod-wlcompat
endef
$(eval $(call Profile,Broadcom))

define Profile/BroadcomMimo
  NAME:=Generic, Broadcom WiFi (MIMO)
  PACKAGES:=kmod-brcm-wl-mimo wlc nas kmod-wlcompat
endef
$(eval $(call Profile,BroadcomMimo))

define Profile/Atheros
  NAME:=Generic, Atheros WiFi
  PACKAGES:=kmod-madwifi
endef
$(eval $(call Profile,Atheros))

define Profile/None
  NAME:=Generic, No WiFi
  PACKAGES:=
endef
$(eval $(call Profile,None))


define Profile/WRT54G3G
  NAME:=Linksys WRT54G3G
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-pcmcia-core kmod-pcmcia-serial kmod-nozomi comgt wlc nas kmod-wlcompat
endef
$(eval $(call Profile,WRT54G3G))

define Profile/WL500G
  NAME:=ASUS WL-500g
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-lp wlc nas kmod-wlcompat
endef
$(eval $(call Profile,WL500G))

define Profile/WL500GD
  NAME:=ASUS WL-500g Deluxe
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-uhci kmod-usb2 wlc nas kmod-wlcompat
endef
$(eval $(call Profile,WL500GD))

define Profile/WL500GP
  NAME:=ASUS WL-500g Premium
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 wlc nas kmod-wlcompat
endef
$(eval $(call Profile,WL500GP))

define Profile/WLHDD
  NAME:=WL-HDD
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-ide-core kmod-ide-pdc202xx wlc nas kmod-wlcompat
endef
$(eval $(call Profile,WLHDD))

$(eval $(call BuildKernel))
