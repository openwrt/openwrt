# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org
#

include $(TOPDIR)/rules.mk

PKG_NAME:=amlogic-boot-fip
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/LibreELEC/amlogic-boot-fip.git
PKG_SOURCE_DATE:=2025-04-17
PKG_SOURCE_VERSION:=42d372123631066fb77fbcbb612dc3eb41a3f6f9
PKG_MIRROR_HASH:=cbbca799239002430486c83a644d0e9807dfcaa95985344d010bb18d357aec36

PKG_MAINTAINER:=OpenWrt Developers <openwrt-devel@lists.openwrt.org>
PKG_LICENSE:=LicenseRef-Amlogic-SLA
PKG_LICENSE_FILES:=LICENSE

PKG_HOST_ONLY:=1
include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/amlogic-boot-fip
  SECTION:=boot
  CATEGORY:=Boot Loaders
  TITLE:=Amlogic boot FIP binaries
  URL:=https://github.com/LibreELEC/amlogic-boot-fip
  BUILDONLY:=1
endef

define Package/amlogic-boot-fip/description
  Signed Amlogic bootloader binaries and build-fip.sh script used to
  create bootable SD/eMMC images for Amlogic Meson GXBB/GXL/GXM SoCs.
endef

define Host/Compile
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/share/amlogic-boot-fip
	$(CP) $(HOST_BUILD_DIR)/* $(STAGING_DIR_HOST)/share/amlogic-boot-fip/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,amlogic-boot-fip))
