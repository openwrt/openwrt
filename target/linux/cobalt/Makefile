#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=cobalt
BOARDNAME:=Cobalt Microservers
FEATURES:=targz pci ext4 display
CPU_TYPE := 5k
CPU_CFLAGS_5k := -march=r5000
MAINTAINER:=Florian Fainelli <florian@openwrt.org>

KERNEL_PATCHVER:=3.3

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Cobalt servers systems (Qube/Qube2/RaQ/RaQ2).
endef

$(eval $(call BuildTarget))
