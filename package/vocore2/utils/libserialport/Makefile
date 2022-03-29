include $(TOPDIR)/rules.mk

PKG_NAME:=libserialport
PKG_VERSION:=0.1.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://sigrok.org/download/source/libserialport/$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=b93f0325a6157198152b5bd7e8182b51

include $(INCLUDE_DIR)/package.mk

define Package/libserialport
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=A library for serial port.
endef

define Package/libserialport/description
  A library for serial port.
endef

define Package/libserialport/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/.libs/libserialport.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libserialport))
