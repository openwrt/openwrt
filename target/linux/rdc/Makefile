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
SUBTARGETS:=amit ar525w r8610 dir-450 sitecom

LINUX_VERSION:=2.6.30.9

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += hostapd-mini kmod-rdc321x-wdt kmod-r6040

define Target/Description
        Build firmware images for RDC321x based routers
        (e.g. Airlink101 AR525W, Linksys WRT54GR, Sitecom WL-153)
endef

$(eval $(call BuildTarget))

$(eval $(call RequireCommand,lzma, \
        Please install lzma-4.4x. \
))
