include $(TOPDIR)/rules.mk

PKG_NAME:=yafut
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/kempniu/yafut.git
PKG_MIRROR_HASH:=f5c76edc81477d2b68a7b032487d10fa361c8eaeecfc36908fde2ae828b7e822
PKG_SOURCE_DATE:=2025-02-18
PKG_SOURCE_VERSION:=e342c93981dc255bba58c17925b5c8983d7dacf8

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
