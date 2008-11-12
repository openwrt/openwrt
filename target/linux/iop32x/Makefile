# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=iop32x
BOARDNAME:=Intel IOP32x
FEATURES:=squashfs jffs2

LINUX_VERSION:=2.6.26.8

include $(INCLUDE_DIR)/target.mk

define Kernel/Configure
	$(call Kernel/Configure/Default)
	$(SED) 's,.*CONFIG_AEABI.*,$(if $(CONFIG_EABI_SUPPORT),CONFIG_AEABI=y,# CONFIG_AEABI is not set),' $(LINUX_DIR)/.config
	$(if $(CONFIG_EABI_SUPPORT),echo '# CONFIG_OABI_COMPAT is not set' >> $(LINUX_DIR)/.config)
endef

$(eval $(call BuildTarget))
