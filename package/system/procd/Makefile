include $(TOPDIR)/rules.mk

PKG_NAME:=procd
PKG_VERSION:=2014-06-17

PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/luci2/procd.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=ded9fec7e4afae14fe2821608c132325afd65b7e
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
  DEPENDS:=+ubusd +ubus +libjson-script +ubox +USE_EGLIBC:librt +libubox +libubus +NAND_SUPPORT:procd-nand
  TITLE:=OpenWrt system process manager
endef

define Package/procd-nand
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@NAND_SUPPORT @mips +ubi-utils
  TITLE:=OpenWrt system process manager
endef

define Package/procd/install
	$(INSTALL_DIR) $(1)/sbin $(1)/etc $(1)/lib/functions

	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/{init,procd,askfirst,udevtrigger} $(1)/sbin/
	$(INSTALL_BIN) ./files/reload_config $(1)/sbin/
	$(INSTALL_DATA) ./files/hotplug*.json $(1)/etc/
	$(INSTALL_DATA) ./files/procd.sh $(1)/lib/functions/
endef

define Package/procd-nand/install
	$(INSTALL_DIR) $(1)/sbin $(1)/lib/upgrade

	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/upgraded $(1)/sbin/
	$(INSTALL_DATA) ./files/nand.sh $(1)/lib/upgrade/
endef

$(eval $(call BuildPackage,procd))
$(eval $(call BuildPackage,procd-nand))
