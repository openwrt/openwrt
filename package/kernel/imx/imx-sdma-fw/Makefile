#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=imx-sdma-fw
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/imx-sdma-fw-$(PKG_VERSION)
PKG_SOURCE:=imx-sdma-fw-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources/
PKG_MD5SUM:=49737c5fbee685cc1de2542c7663006d
PKG_MAINTAINER:=Luka Perkov <luka@openwrt.org>

include $(INCLUDE_DIR)/package.mk

define Package/kmod-imx-sdma-fw-template
  TITLE+=SDMA firmware for i.MX $(1) SoC
  CATEGORY:=Kernel modules
  SUBMENU:=Other modules
  VARIANT:= imx$(1)
  DEPENDS:=@TARGET_imx6
endef

define Package/kmod-imx-sdma-fw-template/description
 SDMA firmware for Freescale i.MX SoC

 http://comments.gmane.org/gmane.linux.ports.arm.kernel/210985
endef

Package/kmod-imx-sdma-fw-imx25=$(call Package/kmod-imx-sdma-fw-template,25)
Package/kmod-imx-sdma-fw-imx31=$(call Package/kmod-imx-sdma-fw-template,31)
Package/kmod-imx-sdma-fw-imx35=$(call Package/kmod-imx-sdma-fw-template,35)
Package/kmod-imx-sdma-fw-imx51=$(call Package/kmod-imx-sdma-fw-template,51)
Package/kmod-imx-sdma-fw-imx53=$(call Package/kmod-imx-sdma-fw-template,53)
Package/kmod-imx-sdma-fw-imx6=$(call Package/kmod-imx-sdma-fw-template,6)

define Build/Compile
endef

define Package/kmod-imx-sdma-fw-$(BUILD_VARIANT)/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(CP) $(PKG_BUILD_DIR)/sdma-$(BUILD_VARIANT).bin $(1)/lib/firmware/
endef

$(eval $(call BuildPackage,kmod-imx-sdma-fw-imx25))
$(eval $(call BuildPackage,kmod-imx-sdma-fw-imx31))
$(eval $(call BuildPackage,kmod-imx-sdma-fw-imx35))
$(eval $(call BuildPackage,kmod-imx-sdma-fw-imx51))
$(eval $(call BuildPackage,kmod-imx-sdma-fw-imx53))
$(eval $(call BuildPackage,kmod-imx-sdma-fw-imx6))
