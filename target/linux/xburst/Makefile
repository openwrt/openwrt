#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=xburst
BOARDNAME:=Ingenic XBurst
FEATURES:=jffs2 tgz ubifs audio
SUBTARGETS:=qi_lb60 n516 n526

LINUX_VERSION:=2.6.32.9

DEVICE_TYPE=other

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=uImage

define Target/Description
	Build firmware images for XBurst JZ47x0 based boards.
endef

$(eval $(call BuildTarget))
