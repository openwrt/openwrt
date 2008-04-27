# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=pxa
BOARDNAME:=Marvell/Intel PXA2xx
FEATURES:=jffs2 broken

LINUX_VERSION:=2.6.21.7

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build images for PAX2xx systems, eg. Gumstix.
endef

define Kernel/Configure
	$(call Kernel/Configure/Default)
	$(SED) 's,.*CONFIG_AEABI.*,$(if $(CONFIG_EABI_SUPPORT),CONFIG_AEABI=y,# CONFIG_AEABI is not set),' $(LINUX_DIR)/.config
	$(if $(CONFIG_EABI_SUPPORT),echo '# CONFIG_OABI_COMPAT is not set' >> $(LINUX_DIR)/.config)
endef

KERNELNAME:="uImage"

$(eval $(call BuildTarget))
