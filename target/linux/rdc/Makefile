#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=i386
BOARD:=rdc
BOARDNAME:=RDC 321x
FEATURES:=squashfs jffs2 pci usb pcmcia
CFLAGS:=-Os -pipe -march=i486 -funit-at-a-time
SUBTARGETS:=ar525w r8610 sitecom bifferboard

LINUX_VERSION:=2.6.30.10

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += wpad-mini kmod-r6040 kmod-input-core \
		    kmod-input-polldev kmod-input-gpio-buttons kmod-button-hotplug

define Target/Description
        Build firmware images for RDC321x based routers
        (e.g. Airlink101 AR525W, Linksys WRT54GR, Sitecom WL-153)
endef

$(eval $(call BuildTarget))
