# Copyright (C) Zy143L
# rebuid by Manper

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-zmodem
PKG_VERSION:=1.7
PKG_RELEASE:=1

LUCI_TITLE:=LuCI for zmodem
LUCI_PKGARCH:=all
LUCI_DEPENDS:=+kmod-usb-net +kmod-usb-core +kmod-usb-ohci +kmod-usb-serial +kmod-usb-serial-option +kmod-usb-uhci +kmod-usb2 +usb-modeswitch +usbutils


include $(TOPDIR)/feeds/luci/luci.mk

# $(eval $(call BuildPackage,luci-app-zmodem))
