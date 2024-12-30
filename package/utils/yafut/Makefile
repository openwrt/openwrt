include $(TOPDIR)/rules.mk

PKG_NAME:=yafut
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/kempniu/yafut.git
PKG_MIRROR_HASH:=7540e977104d41b3aca27b58fda8cd84ebec80cfe01d955712fb8dc717aff6a6
PKG_SOURCE_DATE:=2024-06-10
PKG_SOURCE_VERSION:=38439f8a53d33b14744bc8f938662670b9d3e361

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_INSTALL:=1

define Package/yafut
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Yet Another File UTility
  DEPENDS:=@NAND_SUPPORT
endef

define Package/yafut/description
  A program for copying files from/to Yaffs file systems from userspace.
endef

define Package/yafut/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/yafut $(1)/usr/bin
endef

$(eval $(call BuildPackage,yafut))
