#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=i386
BOARD:=rdc
BOARDNAME:=RDC 321x
FEATURES:=squashfs jffs2 pci usb pcmcia
CFLAGS:=-Os -pipe -march=i486 -mtune=i486 -funit-at-a-time

LINUX_VERSION:=2.6.32.32

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += wpad-mini kmod-libphy kmod-r6040 kmod-input-core \
		    kmod-input-polldev kmod-input-gpio-buttons kmod-button-hotplug \
		    kmod-rdc321x-wdt

define Target/Description
        Build firmware images for RDC321x based routers
        (e.g. Airlink101 AR525W, Linksys WRT54GR, Sitecom WL-153)
endef

$(eval $(call BuildTarget))
