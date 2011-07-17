# Copyright (C) 2009-2011 OpenWrt.org
# All rights reserved.

include $(TOPDIR)/rules.mk

PKG_NAME:=mountd
PKG_VERSION:=0.1
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
PKG_MD5SUM:=b77253ee4321d24d200fffc4f7ca3d15

include $(INCLUDE_DIR)/package.mk

define Package/mountd
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OpenWrt automount daemon
  DEPENDS:=@USB_SUPPORT +uci +kmod-usb-storage +kmod-fs-autofs4
  URL:=http://www.openwrt.org
endef

define Package/mountd/description
  openwrt automount daemon
endef

define Package/mountd/conffiles
/etc/config/mountd
endef

define Build/Compile
	$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -I$(PKG_BUILD_DIR)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/mountd/install
	$(INSTALL_DIR) $(1)/sbin/ $(1)/etc/config/ $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mountd $(1)/sbin/
	$(INSTALL_DATA) ./files/mountd.config $(1)/etc/config/mountd
	$(INSTALL_BIN) ./files/mountd.init $(1)/etc/init.d/mountd
endef

$(eval $(call BuildPackage,mountd))
