include $(TOPDIR)/rules.mk

PKG_NAME:=udevmand
PKG_RELEASE:=1

#PKG_SOURCE_PROTO:=git
#PKG_SOURCE_URL=https://github.com/blogic/udevmand.git
PKG_SOURCE_DATE:=20200623
PKG_SOURCE_VERSION:=5115b6ff4187e344767dae21daf1cbc210bb03fb
CMAKE_INSTALL:=1

PKG_LICENSE:=LGPL-2.1
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/udevmand
  SECTION:=ucentral
  CATEGORY:=uCentral
  DEPENDS:=+libubus +libblobmsg-json +ubusd +libjson-c +libnl-tiny +libuci
  TITLE:=Network devman mapping daemon
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include \
	-I$(STAGING_DIR)/usr/include/libnl-tiny

define Package/udevmand/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) ./files/* $(1)/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/udevmand $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,udevmand))
