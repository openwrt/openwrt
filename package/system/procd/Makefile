include $(TOPDIR)/rules.mk

PKG_NAME:=procd
PKG_VERSION:=2013-11-13

PKG_RELEASE=$(PKG_SOURCE_VERSION)-1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/luci2/procd.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=f9d31edb8938341b9217ee4c14eb58111414eb97
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

TARGET_LDFLAGS += $(if $(CONFIG_USE_EGLIBC),-lrt)

define Package/procd
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+ubusd +ubus +libjson-script +ubox +USE_EGLIBC:librt
  TITLE:=OpenWrt system process manager
endef

define Package/procd/install
	$(INSTALL_DIR) $(1)/sbin $(1)/lib/functions $(1)/etc/init.d $(1)/lib

	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/{procd,askfirst,udevtrigger,logread,validate_data} $(1)/sbin/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libvalidate.so $(1)/lib/
	$(INSTALL_BIN) ./files/reload_config $(1)/sbin/
	$(INSTALL_BIN) ./files/log.init $(1)/etc/init.d/log
	$(INSTALL_DATA) ./files/hotplug*.json $(1)/etc/
	$(INSTALL_DATA) ./files/procd.sh ./files/procd-validate.sh $(1)/lib/functions/
	ln -s /sbin/procd $(1)/sbin/init
endef

$(eval $(call BuildPackage,procd))
