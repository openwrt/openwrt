#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uboot-envtools
PKG_DISTNAME:=u-boot
PKG_VERSION:=2013.10
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.denx.de/pub/u-boot
PKG_MD5SUM:=a076a044b64371edc52f7e562b13f6b2

PKG_BUILD_DIR:=$(BUILD_DIR)/u-boot-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/uboot-envtools
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=read/modify U-Boot bootloader environment
  URL:=http://www.denx.de/wiki/U-Boot
endef

define Package/uboot-envtools/description
 This package includes tools to read and modify U-Boot bootloader environment.
endef

define Build/Configure
endef

define Build/Compile
	touch $(PKG_BUILD_DIR)/include/config.h
	$(MAKE) -C $(PKG_BUILD_DIR) \
		HOSTCC="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)gcc" \
		HOSTSTRIP="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)strip" \
		env
endef

define Package/uboot-envtools/conffiles
/etc/config/ubootenv
/etc/fw_env.config
endef

define Package/uboot-envtools/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/env/fw_printenv $(1)/usr/sbin
	ln -s fw_printenv $(1)/usr/sbin/fw_setenv
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_DATA) ./files/uboot-envtools.sh $(1)/lib
ifneq ($(CONFIG_TARGET_ar71xx),)
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/ar71xx $(1)/etc/uci-defaults/30_uboot-envtools
endif
ifneq ($(CONFIG_TARGET_cns3xxx),)
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/cns3xxx $(1)/etc/uci-defaults/30_uboot-envtools
endif
ifneq ($(CONFIG_TARGET_imx6),)
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/imx6 $(1)/etc/uci-defaults/30_uboot-envtools
endif
ifneq ($(CONFIG_TARGET_kirkwood),)
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/kirkwood $(1)/etc/uci-defaults/30_uboot-envtools
endif
ifneq ($(CONFIG_TARGET_lantiq),)
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/lantiq $(1)/etc/uci-defaults/30_uboot-envtools
endif
ifneq ($(CONFIG_TARGET_ramips),)
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/ramips $(1)/etc/uci-defaults/30_uboot-envtools
endif
endef

$(eval $(call BuildPackage,uboot-envtools))
