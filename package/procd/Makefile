include $(TOPDIR)/rules.mk

PKG_NAME:=procd
PKG_VERSION:=2013-03-14
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/luci2/procd.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=9fcc900e301e0c304488b6b83f10c8db5b8f4ce0
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_CONFIG_DEPENDS:=CONFIG_PROCD_INIT

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/procd
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+ubusd +ubus +libjson-script
  TITLE:=OpenWrt system process manager
endef

ifneq ($(CONFIG_PROCD_INIT),)
define Package/procd/install
	$(INSTALL_DIR) $(1)/sbin $(1)/lib/functions $(1)/etc

	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/{procd,askfirst,udevtrigger,logread} $(1)/sbin/
	$(INSTALL_DATA) ./files/hotplug*.json $(1)/etc/
	$(INSTALL_DATA) ./files/procd.sh $(1)/lib/functions/
	ln -s /sbin/procd $(1)/sbin/init
endef
else
define Package/procd/install
	$(INSTALL_DIR) $(1)/sbin $(1)/lib/functions $(1)/etc/init.d

	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/procd $(1)/sbin/
	$(INSTALL_BIN) ./files/procd.init $(1)/etc/init.d/procd
	$(INSTALL_DATA) ./files/procd.sh $(1)/lib/functions/
endef
endif

$(eval $(call BuildPackage,procd))
