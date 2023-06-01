# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=firmware-utils
PKG_RELEASE:=2

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/firmware-utils.git
PKG_SOURCE_DATE:=2023-05-18
PKG_SOURCE_VERSION:=02cdbc6a4d61605c008efef09162f772f553fcde
PKG_MIRROR_HASH:=f5188fc38bb03ddbcc34763ff049597e2d8af98c0854910dc87f10e5927096e2

PKG_BUILD_DEPENDS:=openssl zlib

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/default
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Firmware utility $(1)
  DEPENDS:=$(2)
endef

Package/oseama = $(call Package/default,oseama,@TARGET_bcm53xx)

define Package/oseama/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/oseama $(1)/usr/bin/
endef

Package/otrx = $(call Package/default,otrx,@(TARGET_bcm47xx||TARGET_bcm53xx))

define Package/otrx/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/otrx $(1)/usr/bin/
endef

$(eval $(call BuildPackage,oseama))
$(eval $(call BuildPackage,otrx))
