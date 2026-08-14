# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=display
PKG_VERSION:=2026.08.14
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=857574ae02dc11943f99423419e71556775977bd
PKG_SOURCE:=hiveton-mt-lvgl-c-$(PKG_SOURCE_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/Hiveton/hiveton-mt-lvgl-c/tar.gz/$(PKG_SOURCE_VERSION)?
PKG_HASH:=7d9704c4f9365ddfb59c383cf625c1acc9725bd4c20670926434fa1992d958ff
PKG_BUILD_DIR:=$(BUILD_DIR)/hiveton-mt-lvgl-c-$(PKG_SOURCE_VERSION)

# The upstream application currently has no top-level license declaration.
PKG_LICENSE:=UNKNOWN
PKG_LICENSE_FILES:=

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

# Upstream does not provide an application install rule; package directly from bin/.
define Build/Install
endef

define Package/display
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=E87N LVGL status display
  URL:=https://github.com/Hiveton/hiveton-mt-lvgl-c
  DEPENDS:=@aarch64 +libfreetype +libstdcpp +libgcc
endef

define Package/display/description
  LVGL status display for the E87N NV3007 panel, built from the pinned
  Hiveton source tree with dynamic NVMe HWMON temperature support.
endef

define Package/display/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/display $(1)/usr/sbin/display
endef

$(eval $(call BuildPackage,display))
