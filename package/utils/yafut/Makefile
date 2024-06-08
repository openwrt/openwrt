include $(TOPDIR)/rules.mk

PKG_NAME:=yafut
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/kempniu/yafut.git
PKG_MIRROR_HASH:=9cc6b4c485ce16d44b67ebf79e8bee1e07aecde112da739cf33e8714ac3842e7
PKG_SOURCE_DATE:=2024-05-13
PKG_SOURCE_VERSION:=2b45baaf1cced47af8f22dd3acbf1df2f04c7510

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

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
