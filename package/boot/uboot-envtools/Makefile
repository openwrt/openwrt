#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uboot-envtools
PKG_DISTNAME:=u-boot
PKG_VERSION:=2024.07
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_DISTNAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
    https://ftp.denx.de/pub/u-boot \
    https://mirror.cyberbits.eu/u-boot \
    ftp://ftp.denx.de/pub/u-boot
PKG_HASH:=f591da9ab90ef3d6b3d173766d0ddff90c4ed7330680897486117df390d83c8f
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_DISTNAME)-$(PKG_VERSION)

PKG_BUILD_DEPENDS:=fstools

PKG_LICENSE:=GPL-2.0 GPL-2.0+
PKG_LICENSE_FILES:=Licenses/README

PKG_FLAGS:=nonshared

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/uboot-envtools
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Boot Loaders
  TITLE:=read/modify U-Boot bootloader environment
  URL:=http://www.denx.de/wiki/U-Boot
endef

define Package/uboot-envtools/description
 This package includes tools to read and modify U-Boot bootloader environment.
endef

define Build/Configure
	$(call Build/Compile/Default,tools-only_defconfig)
endef

define Build/Compile
	$(call Build/Compile/Default,envtools)
endef

# We need to override the ARCH passed by buildsystem as otherwise the defconfig
# for tools-only wont match and the includes for sandbox will be dropped
MAKE_FLAGS += \
	ARCH="sandbox" \
	TARGET_CFLAGS="$(TARGET_CFLAGS)" \
	TARGET_LDFLAGS="$(TARGET_LDFLAGS)"

define Package/uboot-envtools/conffiles
/etc/config/ubootenv
/etc/fw_env.config
/etc/fw_sys.config
endef

define Package/uboot-envtools/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/env/fw_printenv $(1)/usr/sbin
	$(LN) fw_printenv $(1)/usr/sbin/fw_setenv
	$(INSTALL_BIN) ./files/fw_printsys $(1)/usr/sbin
	$(INSTALL_BIN) ./files/fw_setsys $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_DATA) ./files/uboot-envtools.sh $(1)/lib
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(if $(wildcard ./files/$(BOARD)_$(SUBTARGET)), \
		$(INSTALL_DATA) ./files/$(BOARD)_$(SUBTARGET) \
		$(1)/etc/uci-defaults/30_uboot-envtools, \
		$(if $(wildcard ./files/$(BOARD)), \
			$(INSTALL_DATA) ./files/$(BOARD) \
			$(1)/etc/uci-defaults/30_uboot-envtools \
		) \
	)
endef

$(eval $(call BuildPackage,uboot-envtools))
