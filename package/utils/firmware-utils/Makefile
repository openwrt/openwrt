# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=firmware-utils
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/firmware-utils.git
PKG_SOURCE_DATE:=2024-06-20
PKG_SOURCE_VERSION:=6ac44974185a3e7dc7848e97b964339948e817a7
PKG_MIRROR_HASH:=ee5b29f45593750a6806cfa7cad3fd766b321b44107a6b481b890efe82a7dbf5

PKG_FLAGS:=nonshared
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
