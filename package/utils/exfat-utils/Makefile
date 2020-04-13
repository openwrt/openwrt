include $(TOPDIR)/rules.mk

PKG_NAME:=exfat-utils
PKG_VERSION:=1.0.1
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/$(PKG_NAME)/$(PKG_NAME)/tar.gz/$(PKG_VERSION)?
PKG_HASH:=7d4e95c3042b58601a581dc02f55eb4a726a2fb8db2e74a9619dbf3083997e06

PKG_FIXUP:=autoreconf
PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

PKG_MAINTAINER:=Daniel Golle <daniel@makrotopia.org>

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-shared \
	--disable-static

define Package/exfat-utils/Default
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  DEPENDS:=+libexfat
  URL:=https://github.com/exfat-utils/exfat-utils
endef

define Package/exfat-mkfs
  $(Package/exfat-utils/Default)
  TITLE:=Utility for creating an exFAT File System
endef

define Package/exfat-fsck
  $(Package/exfat-utils/Default)
  TITLE:=Utility for checking/repairing an exFAT File System
endef

define Package/libexfat
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Library for exFAT File System tools
  DEPENDS:=+libuuid
  ABI_VERSION:=1
endef

define Package/libexfat/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libexfat.so.* $(1)/usr/lib/
endef

define Package/exfat-mkfs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/mkfs.exfat $(1)/usr/sbin
endef

define Package/exfat-fsck/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/fsck.exfat $(1)/usr/sbin
endef

$(eval $(call BuildPackage,libexfat))
$(eval $(call BuildPackage,exfat-mkfs))
$(eval $(call BuildPackage,exfat-fsck))
