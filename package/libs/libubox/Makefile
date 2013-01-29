include $(TOPDIR)/rules.mk

PKG_NAME:=libubox
PKG_VERSION:=2013-01-29
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/luci2/libubox.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=0bc317aa4d9af44806c28ca286d79a8b5a92b2b8
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_MIRROR_MD5SUM:=c79ef80b71127e114d73701fbdc895c3
CMAKE_INSTALL:=1

PKG_LICENSE:=ISC BSD-3c
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

PKG_BUILD_DEPENDS:=lua

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libubox
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Basic utility library
  DEPENDS:=
endef

define Package/libblobmsg-json
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=blobmsg <-> json conversion library
  DEPENDS:=+libjson +libubox
endef

define Package/jshn
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libjson
  TITLE:=JSON SHell Notation
endef

define Package/jshn/description
  Library for parsing and generating JSON from shell scripts
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include
CMAKE_OPTIONS = \
	-DLUAPATH=/usr/lib/lua

define Package/libubox/install
	$(INSTALL_DIR) $(1)/lib/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libubox.so $(1)/lib/
endef

define Package/libblobmsg-json/install
	$(INSTALL_DIR) $(1)/lib/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libblobmsg_json.so $(1)/lib/
endef

define Package/jshn/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/usr/share/libubox
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/jshn $(1)/usr/bin
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/share/libubox/jshn.sh $(1)/usr/share/libubox
endef

$(eval $(call BuildPackage,libubox))
$(eval $(call BuildPackage,libblobmsg-json))
$(eval $(call BuildPackage,jshn))

