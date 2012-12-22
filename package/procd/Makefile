include $(TOPDIR)/rules.mk

PKG_NAME:=procd
PKG_VERSION:=2012-12-20
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/luci2/procd.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=d343dd9e9a64d4ae7d225ea29169e97fa8d116a1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/procd
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+ubusd +ubus
  TITLE:=OpenWrt system process manager
endef

define Package/procd/install
	$(INSTALL_DIR) $(1)/sbin $(1)/lib/functions $(1)/etc/init.d

	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/procd $(1)/sbin/
	$(INSTALL_BIN) ./files/procd.init $(1)/etc/init.d/procd
	$(INSTALL_DATA) ./files/procd.sh $(1)/lib/functions/
endef

$(eval $(call BuildPackage,procd))
